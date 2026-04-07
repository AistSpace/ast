///
/// @file      ObjectLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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

#include "ObjectLoader.hpp"
#include "AstCore/CelestialBodyLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstSim/Mover.hpp"
#include "AstSim/Spacecraft.hpp"
#include "AstSim/MoverLoader.hpp"
#include "AstSim/Facility.hpp"
#include "AstSim/FacilityLoader.hpp"
#include "AstSim/LineTarget.hpp"
#include "AstSim/MTO.hpp"
#include "AstSim/Place.hpp"
#include "AstSim/Planet.hpp"
#include "AstSim/Star.hpp"
#include "AstSim/Target.hpp"
#include "AstSim/Volumetric.hpp"
#include "AstSim/AdvCAT.hpp"
#include "AstSim/AreaTarget.hpp"
#include "AstSim/Chain.hpp"
#include "AstSim/CommSystem.hpp"
#include "AstSim/Constellation.hpp"
#include "AstSim/CoverageDefinition.hpp"
#include "AstCore/CelestialBodyLoader.hpp"
#include "LineTargetLoader.hpp"
#include "MTOLoader.hpp"
#include "PlaceLoader.hpp"
#include "TargetLoader.hpp"
#include "VolumetricLoader.hpp"
#include "AdvCATLoader.hpp"
#include "AreaTargetLoader.hpp"
#include "ChainLoader.hpp"
#include "CommSystemLoader.hpp"
#include "ConstellationLoader.hpp"
#include "CoverageDefinitionLoader.hpp"


AST_NAMESPACE_BEGIN

errc_t aLoadObject(StringView filepath, StringView objectType, SharedPtr<Object> &object)
{
    if(
        aEqualsIgnoreCase(objectType, "Satellite") || 
        aEqualsIgnoreCase(objectType, "Spacecraft") ||
        aEqualsIgnoreCase(objectType, "Aircraft") ||
        aEqualsIgnoreCase(objectType, "GroundVehicle") || 
        aEqualsIgnoreCase(objectType, "LaunchVehicle") ||
        aEqualsIgnoreCase(objectType, "Missile") ||
        aEqualsIgnoreCase(objectType, "Ship") ||
        aEqualsIgnoreCase(objectType, "Submarine")
    )
    {
        auto mover = new Mover();
        object = mover;
        return aLoadMover(filepath, *mover);
    }
    else if(aEqualsIgnoreCase(objectType, "Facility"))
    {
        auto facility = new Facility();
        object = facility;
        return aLoadFacility(filepath, *facility);
    }
    else if(aEqualsIgnoreCase(objectType, "LineTarget"))
    {
        auto lineTarget = new LineTarget();
        object = lineTarget;
        return aLoadLineTarget(filepath, *lineTarget);
    }
    else if(aEqualsIgnoreCase(objectType, "MTO"))
    {
        auto mto = new MTO();
        object = mto;
        return aLoadMTO(filepath, *mto);
    }
    else if(aEqualsIgnoreCase(objectType, "Place"))
    {
        auto place = new Place();
        object = place;
        return aLoadPlace(filepath, *place);
    }
    else if(aEqualsIgnoreCase(objectType, "Planet"))
    {
        HCelestialBody planet;
        errc_t rc = aLoadBody(filepath, planet);
        if(rc == eNoError)
        {
            object = planet;
        }
        return rc;
    }
    else if(aEqualsIgnoreCase(objectType, "Star"))
    {
        HCelestialBody star;
        errc_t rc = aLoadBody(filepath, star);
        if(rc == eNoError)
        {
            object = star;
        }
        return rc;
    }
    else if(aEqualsIgnoreCase(objectType, "Target"))
    {
        auto target = new Target();
        object = target;
        return aLoadTarget(filepath, *target);
    }
    else if(aEqualsIgnoreCase(objectType, "Volumetric"))
    {
        auto volumetric = new Volumetric();
        object = volumetric;
        return aLoadVolumetric(filepath, *volumetric);
    }
    else if(aEqualsIgnoreCase(objectType, "AdvCAT"))
    {
        auto advCAT = new AdvCAT();
        object = advCAT;
        return aLoadAdvCAT(filepath, *advCAT);
    }
    else if(aEqualsIgnoreCase(objectType, "AreaTarget"))
    {
        auto areaTarget = new AreaTarget();
        object = areaTarget;
        return aLoadAreaTarget(filepath, *areaTarget);
    }
    else if(aEqualsIgnoreCase(objectType, "Chain"))
    {
        auto chain = new Chain();
        object = chain;
        return aLoadChain(filepath, *chain);
    }
    else if(aEqualsIgnoreCase(objectType, "CommSystem"))
    {
        auto commSystem = new CommSystem();
        object = commSystem;
        return aLoadCommSystem(filepath, *commSystem);
    }
    else if(aEqualsIgnoreCase(objectType, "Constellation"))
    {
        auto constellation = new Constellation();
        object = constellation;
        return aLoadConstellation(filepath, *constellation);
    }
    else if(aEqualsIgnoreCase(objectType, "CoverageDefinition"))
    {
        auto coverageDefinition = new CoverageDefinition();
        object = coverageDefinition;
        return aLoadCoverageDefinition(filepath, *coverageDefinition);
    }
    else
    {
        aError("unsupported object type: '%.*s'", objectType.size(), objectType.data());
        return eErrorInvalidParam;
    }
    return eNoError;
}

AST_NAMESPACE_END