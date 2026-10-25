///
/// @file      Path.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-23
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Path.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include <climits>
#include <cstddef>
#include <cstring>
#include <vector>
#include <map>
#include <mutex>
#ifdef _WIN32
#include <Windows.h>
#include <winioctl.h>   // FSCTL_SET_REPARSE_POINT

// 旧版 SDK 未定义这两个符号链接标志
#ifndef SYMBOLIC_LINK_FLAG_DIRECTORY
#    define SYMBOLIC_LINK_FLAG_DIRECTORY 0x00000001
#endif
#ifndef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
#    define SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE 0x00000002
#endif
#else
#include <cerrno>
#include <unistd.h>
#endif


AST_NAMESPACE_BEGIN

bool aPathIsRelative(StringView path)
{
    return !aPathIsAbsolute(path);
}

bool aPathIsAbsolute(StringView path)
{
    if (path.empty()) return false;

    #ifdef _WIN32
        // UNC 路径 (\\server\share\...)；
        // 只有两个分隔符（\\、//）没有 root_name，std::filesystem 视为相对路径
        if (path.size() >= 3 && aIsPathSeparator(path[0]) && aIsPathSeparator(path[1]))
            return true;

        // 盘符路径 (C:\... 或 C:/...)
        if (path.size() >= 3
            && ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z'))
            && path[1] == ':'
            && aIsPathSeparator(path[2]))
            return true;

        // 只有单个前导分隔符的路径（\foo、/foo）是"根相对"：没有 root_name，
        // std::filesystem 把它当作相对路径（按当前盘解析），这里保持一致
        return false;
    #else
        // POSIX: 以 / 开头即为绝对路径
        return aIsPathSeparator(path[0]);
    #endif
}

#ifdef _WIN32
std::wstring aMakeWin32Path(StringView path)
{
    std::wstring wide_path = aUtf8ToWide(path);  // 转换为宽字符

    // 空路径和短绝对路径原样交给 Win32，避免多余的转换
    // 未加 \\?\ 前缀时，CreateDirectoryW 等 API 在旧版 SDK 中要求路径预留 12 个字符给 8.3 文件名，
    // 所以长度达到 MAX_PATH - 12 就加前缀
    if (wide_path.empty() || (wide_path.size() < MAX_PATH - 12 && aPathIsAbsolute(path)))
        return wide_path;

    // 已带扩展长度前缀（\\?\ 或 \\.\），直接使用
    if (wide_path.size() >= 4 && wide_path.compare(0, 4, LR"(\\?\)") == 0)
        return wide_path;

    // 先查询规范化绝对路径所需的长度
    DWORD size = GetFullPathNameW(wide_path.c_str(), 0, nullptr, nullptr);
    if (size == 0)
        return wide_path;  // 查询失败，保留原路径由调用方报错

    std::wstring full_path(size, L'\0');
    DWORD length = GetFullPathNameW(wide_path.data(), size, &full_path[0], nullptr);
    if (length == 0 || length >= size)
        return wide_path;
    full_path.resize(length);

    if (full_path.size() < MAX_PATH - 12)
        return full_path;

    // UNC 路径（\\server\share\...）需使用 \\?\UNC\ 前缀
    if (full_path.size() >= 2 && full_path[0] == L'\\' && full_path[1] == L'\\')
        return LR"(\\?\UNC\)" + full_path.substr(2);

    return LR"(\\?\)" + full_path;
}
#endif

#ifdef _WIN32
namespace {

/// 目录联接（Junction）的重解析点数据
/// @details 系统头文件未导出 REPARSE_DATA_BUFFER，这里按 MSDN 中
///          REPARSE_DATA_BUFFER 的 MountPointReparseBuffer 分支重新定义
struct JunctionReparseBuffer
{
    ULONG  ReparseTag;              ///< IO_REPARSE_TAG_MOUNT_POINT
    USHORT ReparseDataLength;       ///< 联接数据长度（不含前 8 字节）
    USHORT Reserved;

    // MountPointReparseBuffer
    USHORT SubstituteNameOffset;    ///< 目标路径在 PathBuffer 中的字节偏移
    USHORT SubstituteNameLength;    ///< 目标路径的字节数（不含结尾 NUL）
    USHORT PrintNameOffset;         ///< 显示路径在 PathBuffer 中的字节偏移
    USHORT PrintNameLength;         ///< 显示路径的字节数（不含结尾 NUL）
    WCHAR  PathBuffer[1];           ///< [目标路径 NUL][显示路径 NUL]
};

/// 将创建重解析点（联接点/符号链接）失败的错误码转换为项目错误码
errc_t aReparseError(DWORD error)
{
    switch (error)
    {
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
        return eErrorInvalidParam;      // 链接路径已被占用
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return eErrorNotFound;
    case ERROR_INVALID_FUNCTION:        // 目标卷不支持重解析点（如 FAT32）
    case ERROR_NOT_SUPPORTED:
        return eErrorUnsupported;
    case ERROR_FILENAME_EXCED_RANGE:
        return eErrorOutOfRange;
    default:
        // 包括 ERROR_PRIVILEGE_NOT_HELD：创建符号链接时未开启开发者模式且没有管理员权限
        return eError;
    }
}

/// 把绝对 Win32 路径拆成 NT 路径和显示路径
/// @details
/// 联接点的 SubstituteName 必须是 NT 对象管理器路径：`C:\foo` 对应 `\??\C:\foo`。
/// 联接点只能指向本地卷，UNC 路径（\\server\share）和 `\\.\` 设备路径一律拒绝，
/// 否则会建出一个无法访问的联接。
/// @return 路径不是本地绝对路径时返回 false
bool aMakeJunctionNames(std::wstring path, std::wstring& substituteName, std::wstring& printName)
{
    // NT 路径只接受反斜杠
    for (wchar_t& ch : path)
    {
        if (ch == L'/')
            ch = L'\\';
    }

    // UNC 路径：\\server\share 或 \\?\UNC\server\share
    if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\'
        && !(path.size() >= 4 && path.compare(0, 4, L"\\\\?\\") == 0))
        return false;
    if (path.size() >= 8 && path.compare(0, 8, L"\\\\?\\UNC\\") == 0)
        return false;
    // 扩展长度前缀：\\?\C:\... 或 \\?\Volume{GUID}\...
    if (path.size() >= 4 && path.compare(0, 4, L"\\\\?\\") == 0)
    {
        substituteName = L"\\??\\" + path.substr(4);
        printName = path.substr(4);
        return true;
    }
    // 盘符路径：C:\...
    if (path.size() >= 3
        && ((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z'))
        && path[1] == L':' && path[2] == L'\\')
    {
        substituteName = L"\\??\\" + path;
        printName = path;
        return true;
    }
    return false;
}

} // namespace


errc_t aCreateJunction(StringView targetPath, StringView junctionPath)
{
    if (targetPath.empty() || junctionPath.empty())
        return eErrorInvalidParam;

    // 1. 转换为 Win32 路径，长路径会自动补上 \\?\ 前缀
    std::wstring wideJunction = aMakeWin32Path(junctionPath);
    std::wstring wideTarget = aMakeWin32Path(targetPath);
    if (wideJunction.empty() || wideTarget.empty())
        return eErrorInvalidParam;

    // 2. 计算目标路径的 NT 形式（SubstituteName）与显示形式（PrintName）
    //    aMakeWin32Path 已把相对路径解析为完全限定路径，这里只放行本地绝对路径
    std::wstring substituteName;
    std::wstring printName;
    if (!aMakeJunctionNames(wideTarget, substituteName, printName))
        return eErrorInvalidParam;

    // 3. 创建空目录作为联接点载体；已存在时报错，不覆盖
    if (!CreateDirectoryW(wideJunction.c_str(), nullptr))
        return aReparseError(GetLastError());

    // 4. 打开目录本身，不能跟随重解析点；写入重解析点只需要写权限
    HANDLE hDir = CreateFileW(
        wideJunction.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        nullptr
    );
    if (hDir == INVALID_HANDLE_VALUE)
    {
        errc_t error = aReparseError(GetLastError());
        RemoveDirectoryW(wideJunction.c_str());     // 回滚
        return error;
    }

    // 5. 构造重解析点数据：两个名字依次存放在 PathBuffer 中，各以 NUL 结尾
    const size_t subNameBytes = substituteName.size() * sizeof(wchar_t);
    const size_t printNameBytes = printName.size() * sizeof(wchar_t);
    const size_t pathBufferSize = subNameBytes + sizeof(wchar_t) + printNameBytes + sizeof(wchar_t);
    if (pathBufferSize > USHRT_MAX - 8)             // 偏移和长度都是 16 位字段
    {
        CloseHandle(hDir);
        RemoveDirectoryW(wideJunction.c_str());
        return eErrorOutOfRange;
    }

    const size_t headerSize = offsetof(JunctionReparseBuffer, PathBuffer);
    std::vector<BYTE> buffer(headerSize + pathBufferSize, 0);
    auto* reparse = reinterpret_cast<JunctionReparseBuffer*>(buffer.data());
    reparse->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
    reparse->ReparseDataLength = static_cast<USHORT>(pathBufferSize + 8);
    reparse->Reserved = 0;
    reparse->SubstituteNameOffset = 0;
    reparse->SubstituteNameLength = static_cast<USHORT>(subNameBytes);
    reparse->PrintNameOffset = static_cast<USHORT>(subNameBytes + sizeof(wchar_t));
    reparse->PrintNameLength = static_cast<USHORT>(printNameBytes);
    memcpy(reparse->PathBuffer, substituteName.c_str(), subNameBytes);
    memcpy(reinterpret_cast<BYTE*>(reparse->PathBuffer) + reparse->PrintNameOffset, printName.c_str(), printNameBytes);
    // 结尾的 NUL 已由 buffer 的零初始化填充

    // 6. 写入重解析点
    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(
        hDir,
        FSCTL_SET_REPARSE_POINT,
        reparse, static_cast<DWORD>(buffer.size()),
        nullptr, 0,
        &bytesReturned,
        nullptr
    );
    DWORD lastError = ok ? ERROR_SUCCESS : GetLastError();
    CloseHandle(hDir);

    // 7. 失败时回滚已创建的目录
    if (!ok)
    {
        RemoveDirectoryW(wideJunction.c_str());
        return aReparseError(lastError);
    }
    return eNoError;
}
#endif


errc_t aCreateSymbolicLink(StringView targetPath, StringView linkPath, bool isDirectory)
{
    if (targetPath.empty() || linkPath.empty())
        return eErrorInvalidParam;

    #ifdef _WIN32
    // 1. 链接路径转为 Win32 路径（相对路径解析为绝对路径，长路径补 \\?\ 前缀）；
    //    相对目标路径必须原样保留：aMakeWin32Path 会按"当前目录"把它变成绝对路径，
    //    而符号链接保存的是目标字符串、访问时按"链接所在目录"解析，转换后语义就变了
    std::wstring wideLink = aMakeWin32Path(linkPath);
    std::wstring wideTarget = aPathIsAbsolute(targetPath)
        ? aMakeWin32Path(targetPath)
        : aUtf8ToWide(targetPath);
    if (wideLink.empty() || wideTarget.empty())
        return eErrorInvalidParam;

    // 2. Windows 在创建时就要区分文件链接和目录链接
    DWORD flags = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
    if (isDirectory)
        flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;

    // 3. Windows 10 1703 之前的系统不认识 ALLOW_UNPRIVILEGED_CREATE，
    //    去掉该标志再试一次（此时非管理员会因缺少权限而失败）
    if (CreateSymbolicLinkW(wideLink.c_str(), wideTarget.c_str(), flags))
        return eNoError;
    DWORD error = GetLastError();
    if (error == ERROR_INVALID_PARAMETER)
    {
        flags &= ~SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
        if (CreateSymbolicLinkW(wideLink.c_str(), wideTarget.c_str(), flags))
            return eNoError;
        error = GetLastError();
    }
    return aReparseError(error);
    #else
    // POSIX 的符号链接不区分文件/目录，isDirectory 仅在 Windows 上有效
    (void)isDirectory;

    // StringView 不保证以 '\0' 结尾，先拷贝出独立的字符串
    std::string target(targetPath);
    std::string link(linkPath);
    return ::symlink(target.c_str(), link.c_str());
    #endif
}

// --------------------------------------
// 目录映射器：用于将超长路径映射为短路径别名
// --------------------------------------


namespace{

/// @brief 目录路径映射器
class DirectoryMapper
{
public:
    static DirectoryMapper& Instance();
    DirectoryMapper() = default;
    ~DirectoryMapper() { releaseShortPathAliases(); }   ///< 进程退出时清理别名目录

    std::string getShortPath(StringView path);
protected:
    void releaseShortPathAliases();
public:
    std::mutex mutex_{};                                      ///< 本类被多个模块共用，需要线程安全
    std::map<std::string, std::string> shortPathAliases_{}; ///< 超长父目录 -> 短路径别名目录
    std::string shortPathRoot_{};                           ///< 短路径别名根目录
};

DirectoryMapper& DirectoryMapper::Instance()
{
    static DirectoryMapper instance;
    return instance;
}

std::string DirectoryMapper::getShortPath(StringView dirpath)
{
    if(dirpath.empty())
        return std::string();

    fs::path path(dirpath);

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = shortPathAliases_.find(path.string());
    if(it == shortPathAliases_.end())
    {
        // 别名根目录：临时目录下按进程 + 本对象地址命名
        if(shortPathRoot_.empty())
        {
            std::error_code ec;
            fs::path tempDir = fs::temp_directory_path(ec);
            if(tempDir.empty())
            {
                aWarning(_("无法创建短路径别名目录，直接返回超长路径 '%s'"), path.c_str());
                return path.string();
            }
            shortPathRoot_ = (tempDir / (
                "ast-dirmapper-" + std::to_string(posix::getpid()) + "-"
                + std::to_string(reinterpret_cast<uintptr_t>(this))
            )).string();
            fs::remove_all(shortPathRoot_);     // 清理上次异常退出留下的同名目录
            fs::create_directories(shortPathRoot_, ec);
            if(ec)
            {
                shortPathRoot_.clear();
                aWarning(_("无法创建短路径别名目录，直接返回超长路径 '%s'"), path.c_str());
                return path.string();
            }
        }
        
        fs::path aliasDir = (fs::path(shortPathRoot_) / std::to_string(shortPathAliases_.size()));
        std::error_code ec;

        // 创建符号链接时需要使用绝对路径
        fs::path absDir = fs::absolute(path, ec);
        if(ec)
            return path.string();

        // Windows 用目录联接，创建不需要管理员权限；其它平台用符号链接
        #ifdef _WIN32
        errc_t rc = aCreateJunction(absDir.string(), aliasDir.string());
        #else
        errc_t rc = aCreateSymbolicLink(absDir.string(), aliasDir.string(), true);
        #endif
        if(rc != eNoError)
        {
            aWarning(_("为超长路径 '%s' 创建短路径别名失败，直接返回超长路径 '%s'"), path.c_str(), path.c_str());
            return path.string();
        }
        it = shortPathAliases_.emplace(path, aliasDir).first;
    }

    return it->second;
}

void DirectoryMapper::releaseShortPathAliases()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(shortPathRoot_.empty())
        return;
    // 联接点/符号链接只会删除别名本身，不会删除目标目录里的文件
    fs::remove_all(shortPathRoot_);
    shortPathRoot_.clear();
    shortPathAliases_.clear();
}

}

std::string aTryGetShortDirPath(StringView dirpath)
{
    return DirectoryMapper::Instance().getShortPath(dirpath);
}


std::string aTryGetShortPath(StringView path)
{
    fs::path p(path);
    if(fs::is_directory(p))
        return aTryGetShortDirPath(path);
    fs::path parent = p.parent_path();
    fs::path name = p.filename();
    fs::path newParent = aTryGetShortDirPath(parent.string());
    fs::path newPath = (newParent / name);
    return newPath.string();
}

AST_NAMESPACE_END
