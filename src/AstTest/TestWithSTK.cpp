///
/// @file      TestWithSTK.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
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

#include "TestWithSTK.hpp"
#include "AstUtil/StringView.hpp"
#include "AstSim/Mover.hpp"
#include "AstSim/MoverLoader.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/IO.hpp"
#include "AstMath/UnarySolver.hpp"  // for aIsClose

AST_NAMESPACE_BEGIN

err_t aTestFromSTKFile(StringView filepath)
{
    ast_printf("loading %.*s\n", (int)filepath.size(), filepath.data());
    Mover mover;
    err_t rc = aLoadMover(filepath, mover);
    if(rc != eNoError)
    {
        ast_printf("failed to load %.*s\n", (int)filepath.size(), filepath.data());
        return rc;
    }
    ast_printf("loaded %.*s\n", (int)filepath.size(), filepath.data());
    ast_printf("mover name: %s\n", mover.getName().c_str());

    ast_printf("start testing ephemeris\n");

    auto ephem_readed = mover.getEphemeris();
    if(ephem_readed == nullptr)
    {
        ast_printf("ephemeris is null\n");
        return eErrorParse;
    }
    auto motion = mover.getMotionProfile();
    if(motion == nullptr)
    {
        ast_printf("motion profile is null\n");
        return eErrorParse;
    }
    
    TimeInterval interval;
    rc = ephem_readed->getInterval(interval);
    if(rc != eNoError)
    {
        ast_printf("failed to get interval\n");
        return rc;
    }
    ast_printf("interval: %s\n", interval.toString().c_str());
    
    ast_printf("test specific generated ephemeris...\n");
    ScopedPtr<Ephemeris> ephem_spec_generated;
    rc = motion->makeEphemerisSpec(ephem_spec_generated);
    if(rc != eNoError)
    {
        ast_printf("failed to make specific ephemeris\n");
        return rc;
    }
    ast_printf("specific generated ephemeris generated\n");

    TimePoint stopTime = interval.getStop();
    Vector3d pos, vel;
    Vector3d posExpected, velExpected;
    rc = ephem_spec_generated->getPosVel(stopTime, pos, vel);
    if(rc != eNoError)
    {
        ast_printf("failed to get pos vel from specific generated ephemeris\n");
        return rc;
    }
    rc = ephem_readed->getPosVel(stopTime, posExpected, velExpected);
    if(rc != eNoError)
    {
        ast_printf("failed to get pos vel from readed ephemeris\n");
        return rc;
    }
    ast_printf("pos: %.15g, %.15g, %.15g\n", pos[0], pos[1], pos[2]);
    ast_printf("vel: %.15g, %.15g, %.15g\n", vel[0], vel[1], vel[2]);
    ast_printf("posExpected: %.15g, %.15g, %.15g\n", posExpected[0], posExpected[1], posExpected[2]);
    ast_printf("velExpected: %.15g, %.15g, %.15g\n", velExpected[0], velExpected[1], velExpected[2]);
    for(int i = 0; i < 3; ++i)
    {
        if(!aIsClose(pos[i], posExpected[i], 1e-13))
        {
            ast_printf("pos %d not close\n", i);
            return eErrorInvalidParam;
        }
        if(!aIsClose(vel[i], velExpected[i], 1e-13))
        {
            ast_printf("vel %d not close\n", i);
            return eErrorInvalidParam;
        }
    }
    ast_printf("specific generated ephemeris test passed\n");

    ast_printf("test simple ephemeris...\n");
    ScopedPtr<Ephemeris> ephem_simple_generated;
    rc = motion->makeEphemerisSimple(ephem_simple_generated);
    if(rc != eNoError)
    {
        ast_printf("failed to make simple ephemeris\n");
        return rc;
    }
    ast_printf("simple ephemeris generated\n");
    rc = ephem_simple_generated->getPosVel(stopTime, pos, vel);
    if(rc != eNoError)
    {
        ast_printf("failed to get pos vel from simple ephemeris\n");
        return rc;
    }
    ast_printf("pos: %.15g, %.15g, %.15g\n", pos[0], pos[1], pos[2]);
    ast_printf("vel: %.15g, %.15g, %.15g\n", vel[0], vel[1], vel[2]);
    ast_printf("posExpected: %.15g, %.15g, %.15g\n", posExpected[0], posExpected[1], posExpected[2]);
    ast_printf("velExpected: %.15g, %.15g, %.15g\n", velExpected[0], velExpected[1], velExpected[2]);
    for(int i = 0; i < 3; ++i)
    {
        if(!aIsClose(pos[i], posExpected[i], 1e-13))
        {
            ast_printf("pos %d not close\n", i);
            return eErrorInvalidParam;
        }
        if(!aIsClose(vel[i], velExpected[i], 1e-13))
        {
            ast_printf("vel %d not close\n", i);
            return eErrorInvalidParam;
        }
    }
    ast_printf("simple ephemeris test passed\n");

    ast_printf("all tests passed\n");
    return 0;
}


AST_NAMESPACE_END
