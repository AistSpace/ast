///
/// @file      SpiceApi.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-22
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

#include "SpiceApi.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstCore/RunTimeConfig.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

// 声明spice函数原型，参考SpiceZpr.h
namespace spiceproto{
#if defined(__linux__)
#   define CSPICE_PC_LINUX_64BIT_GCC
#endif

#if (   defined(CSPICE_ALPHA_DIGITAL_UNIX    )      \
     || defined(CSPICE_SUN_SOLARIS_64BIT_NATIVE)    \
     || defined(CSPICE_SUN_SOLARIS_64BIT_GCC )      \
     || defined(CSPICE_MAC_OSX_INTEL_64BIT_GCC )    \
     || defined(CSPICE_MAC_OSX_M1_64BIT_CLANG )     \
     || defined(CSPICE_SUN_SOLARIS_INTEL_64BIT_CC ) \
     || defined(CSPICE_PC_CYGWIN_64BIT_GCC )        \
     || defined(CSPICE_PC_LINUX_64BIT_GCC    )  )
   
    typedef int         SpiceInt;
#else
    typedef long        SpiceInt;
#endif

    typedef const char     ConstSpiceChar;
    typedef double         SpiceDouble;
    typedef char           SpiceChar;
    typedef int            SpiceBoolean;

    
    void              furnsh_c ( ConstSpiceChar    * file );
    
    void              spkgeo_c ( SpiceInt            targ,
                                SpiceDouble         et,
                                ConstSpiceChar    * ref,
                                SpiceInt            obs,
                                SpiceDouble         state[6],
                                SpiceDouble       * lt       );

    void              erract_c ( ConstSpiceChar    * operation,
                                SpiceInt            lenout,
                                SpiceChar         * action    );

    SpiceBoolean      failed_c ( void );

    void              reset_c  ( void );

    // 根据SpiceZdf.h的说明，SpiceInt的大小是SpiceDouble的一半
    // 在这里进行编译期检查
    static_assert(2 * sizeof(SpiceInt) == sizeof(SpiceDouble), "SpiceInt and SpiceDouble size must be 2:1");
}


bool funcarray_isfull(const SpiceApi::funcarray& funcs)
{
    for(auto& func : funcs)
        if(!func)
            return false;
    return true;
}

SpiceApi* SpiceApi::Instance()
{
    static SpiceApi instance_{true};
    return &instance_;
}

SpiceApi::SpiceApi(bool shouldLoadDynamicLib)
{
    if(shouldLoadDynamicLib)
    {
        // 遍历所配置的动态库路径
        auto lispaths = aGetConfigStringVector("CSPICE_LIBS");
        if(!lispaths.empty()){
            tryload(lispaths);
        }else{
            // 尝试加载默认动态库
            load("cspice");
        }
    }
}

SpiceApi::~SpiceApi()
{
    unload();
}

err_t SpiceApi::load(StringView dirpath)
{
    void* lib = aLoadLibrary(std::string(dirpath).c_str());
    if(!lib)
        return eErrorInvalidFile;
    // auto spkpos_ = aGetProcAddress(lib, "spkpos_c");
    funcarray funcs{};
    funcs[ifurnsh] = aGetProcAddress(lib, "furnsh_c");
    funcs[ispkgeo] = aGetProcAddress(lib, "spkgeo_c");
    funcs[ierract] = aGetProcAddress(lib, "erract_c");
    funcs[ifailed] = aGetProcAddress(lib, "failed_c");
    funcs[ireset]  = aGetProcAddress(lib, "reset_c");
    if(!funcarray_isfull(funcs))
    {
        aFreeLibrary(lib);
        return eErrorInvalidFile;
    }
    library_ = lib;
    functions_ = funcs;
    // 防止 cspice 直接让程序退出(默认是ABORT Mode)
    char action[] = "RETURN";
    erract("SET", 0, action);
    return eNoError;
}

err_t SpiceApi::tryload(const std::vector<std::string>& libpaths)
{
    for(auto& libpath : libpaths)
        if(load(libpath) == eNoError)
            return eNoError;
    return eErrorInvalidFile;
}

err_t SpiceApi::unload()
{
    if(library_)
        return aFreeLibrary(library_);
    functions_ = funcarray{};
    return eNoError;
}

err_t SpiceApi::furnsh(const char* libpath)
{
    using functype = decltype(&spiceproto::furnsh_c);
    functype furnsh = reinterpret_cast<functype>(functions_[ifurnsh]);
    if(!furnsh)
    {
        aError("spice library not loaded, call SpiceApi::load first");
        return eErrorNullPtr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    furnsh(libpath);
    return checkerror();
}

err_t SpiceApi::spkgeo(int targ, double et, const char * ref, int obs, double state[6], double * lt)
{
    using functype = decltype(&spiceproto::spkgeo_c);
    functype spkgeo = reinterpret_cast<functype>(functions_[ispkgeo]);
    if(!spkgeo)
    {
        aError("spice library not loaded, call SpiceApi::load first");
        return eErrorNullPtr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    spkgeo(targ, et, ref, obs, state, lt);
    return checkerror();
}

bool SpiceApi::failed()
{
    using functype = decltype(&spiceproto::failed_c);
    functype failed = reinterpret_cast<functype>(functions_[ifailed]);
    if(!failed)
        return true;
    return failed();
}

void SpiceApi::reset()
{
    using functype = decltype(&spiceproto::reset_c);
    functype reset = reinterpret_cast<functype>(functions_[ireset]);
    if(!reset)
        return;
    reset();
}

void SpiceApi::erract(const char * operation, int lenout, char * action)
{
    using functype = decltype(&spiceproto::erract_c);
    functype erract = reinterpret_cast<functype>(functions_[ierract]);
    if(!erract)
        return;
    erract(operation, lenout, action);
}

err_t SpiceApi::checkerror()
{
    if(failed())
    {
        // 这里必须重置一下，否则后续调用会一直报错...
        reset();
        return -1;
    }
    return eNoError;
}



AST_NAMESPACE_END


