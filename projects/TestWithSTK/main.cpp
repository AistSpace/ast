///
/// @file      main.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-24
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstTest/TestWithSTK.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Posix.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/EOP.hpp"

#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    aInitialize();
    aDataContext_GetEOP()->unload();
    if (argc > 1) {
        #ifdef _WIN32
        int argc;
        LPWSTR * wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
        wchar_t* filenameW = wargv[1];
        std::string filenameUTF8;
        aWideToUtf8(filenameW, filenameUTF8);
        const char* filename = filenameUTF8.c_str();
        LocalFree(wargv);
        #else
        const char* filename = argv[1];
        #endif
        return aTestFromSTKFile(filename);
    }else{
        StringView filepath = posix::getenv("TEST_STK_FILE");
        if(!filepath.empty())
            return aTestFromSTKFile(filepath);
        return 0;
    }
}

