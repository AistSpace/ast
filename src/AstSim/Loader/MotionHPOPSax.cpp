///
/// @file      MotionHPOPSax.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
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

#include "MotionHPOPSax.hpp"
#include "AstCore/HPOP.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN


err_t _aLoadSolarPressureModel(BKVParser& parser, double massAtEpoch, HPOPForceModel::SRP& srp)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Coefficient")){
                srp.coefficient_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "Area")){
                srp.areaMassRatio_ = item.value().toDouble() / massAtEpoch;
            }
            else if(aEqualsIgnoreCase(item.key(), "ReflectionModel")){
                // @todo 解析反射模型
            }else if(aEqualsIgnoreCase(item.key(), "CorrectionType")){
                // @todo 解析修正类型
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {

        }else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "SolarPressureModel")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


err_t MotionHPOPSax::keyValue(StringView key, const ValueView &value)
{
    if(aEqualsIgnoreCase(key, "X")){
        cartState_.x() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Y")){
        cartState_.y() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Z")){
        cartState_.z() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VX")){
        cartState_.vx() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VY")){
        cartState_.vy() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VZ")){
        cartState_.vz() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "GravityFile")){
        forceModel_.gravity_.model_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "MaxDegree")){
        forceModel_.gravity_.maxDegree_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MaxOrder")){
        forceModel_.gravity_.maxOrder_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "UseGravSecularVariations")){
        forceModel_.gravity_.useGravSecularVariations_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "SolidTideType")){
        if(aEqualsIgnoreCase(value, "PermanentOnly")){
            forceModel_.gravity_.solidTideType_ = ESolidTideType::ePermanentOnly;
        } else if(aEqualsIgnoreCase(value, "Full")){
            forceModel_.gravity_.solidTideType_ = ESolidTideType::eFull;
        } else if(aEqualsIgnoreCase(value, "None")){
            forceModel_.gravity_.solidTideType_ = ESolidTideType::eNone;
        }else{
            // 
        }
    }
    else if(aEqualsIgnoreCase(key, "IncludeTimeDependSolidTides")){
        forceModel_.gravity_.includeTimeDependSolidTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "MinAmplitudeSolidTides")){
        forceModel_.gravity_.minAmplitudeSolidTides_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "TruncateSolidTides")){
        forceModel_.gravity_.truncateSolidTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "UseOceanTides")){
        forceModel_.gravity_.useSolidTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "MaxDegreeOceanTides")){
        forceModel_.gravity_.maxDegreeOceanTides_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MaxOrderOceanTides")){
        forceModel_.gravity_.maxOrderOceanTides_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MinAmplitudeOceanTides")){
        forceModel_.gravity_.minAmplitudeOceanTides_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "MassAtEpoch")){
        massAtEpoch_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseDrag")){
        forceModel_.useDrag_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "AtmDensityModel")){
        if(aEqualsIgnoreCase(value, "JacchiaRoberts")){
            forceModel_.drag_.atmDensityModel_ = EAtmDensityModel::eJacchiaRoberts;
        } else if(aEqualsIgnoreCase(value, "NRLMSISE2000")){
            forceModel_.drag_.atmDensityModel_ = EAtmDensityModel::eNRLMSISE2000;
        } else if(aEqualsIgnoreCase(value, "MSISE1990")){
            forceModel_.drag_.atmDensityModel_ = EAtmDensityModel::eMSISE1990;
        } else if(aEqualsIgnoreCase(value, "MSIS1986")){
            forceModel_.drag_.atmDensityModel_ = EAtmDensityModel::eMSIS1986;
        } else {
            forceModel_.drag_.atmDensityModel_ = EAtmDensityModel::e1976Standard;
        }
    }
    else if(aEqualsIgnoreCase(key, "AtmBlendingRange")){
        forceModel_.drag_.atmBlendingRange_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseFluxApFile")){
        forceModel_.drag_.useFluxApFile_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "FluxApFile")){
        forceModel_.drag_.fluxApFile_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxSource")){
        if(aEqualsIgnoreCase(value, "Kp")){
            forceModel_.drag_.geoMagneticFluxSource_ = EGeoMagneticFluxSource::eKp;
        } else if(aEqualsIgnoreCase(value, "Ap")){
            forceModel_.drag_.geoMagneticFluxSource_ = EGeoMagneticFluxSource::eAp;
        }else{
            // @todo 处理其他磁通量源
        }
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxUpdateMethod")){
        if(aEqualsIgnoreCase(value, "Daily")){
            forceModel_.drag_.geoMagneticFluxUpdateMethod_ = EGeoMagneticFluxUpdateMethod::eDaily;
        }else{
            // @todo 处理其他更新方法
        }
        // @todo 处理其他更新方法
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxInterpSubSamplingRatio")){
        forceModel_.drag_.geoMagneticFluxInterpSubSamplingRatio_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DragCoefficient")){
        forceModel_.drag_.dragCoefficient_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "AreaMassRatio")){
        forceModel_.drag_.areaMassRatio_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DragCorrectionType")){
        // @todo 处理DragCorrectionType
    }
    else if(aEqualsIgnoreCase(key, "AverageF10.7")){
        forceModel_.drag_.f10p7Average_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DailyF10.7")){
        forceModel_.drag_.f10p7Daily_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Kp")){
        forceModel_.drag_.kp_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseApproxAltForDrag")){
        forceModel_.drag_.useApproxAltForDrag_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "UseApparentSunForDrag")){
        forceModel_.drag_.useApparentSunForDrag_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "DensityType")){
        // @todo 处理DensityType
    }
    else if(aEqualsIgnoreCase(key, "UseSRP")){
        forceModel_.useSRP_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "SolarPressureModel")){
        if(massAtEpoch_ <= 0.0) massAtEpoch_ = 1;
        err_t rc = _aLoadSolarPressureModel(parser_, massAtEpoch_, forceModel_.srp_);
        A_UNUSED(rc);
    }
    else if(aEqualsIgnoreCase(key, "SunPosition")){
        if(aEqualsIgnoreCase(value, "ApparentSunToTrueCB")){
            forceModel_.srp_.sunPosition_ = ESunPosition::eApparentSunToTrueCB;
        } else if(aEqualsIgnoreCase(value, "Apparent")){
            forceModel_.srp_.sunPosition_ = ESunPosition::eApparent;
        } else if(aEqualsIgnoreCase(value, "True")){
            forceModel_.srp_.sunPosition_ = ESunPosition::eTrue;
        }
    }
    else if(aEqualsIgnoreCase(key, "DetectShadowBoundaries")){
        forceModel_.srp_.detectShadowBoundaries_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "ShadowModel")){
        if(aEqualsIgnoreCase(value, "DualCone")){
            forceModel_.srp_.shadowModel_ = EShadowModel::eDualCone;
        } else if(aEqualsIgnoreCase(value, "Cylindrical")){
            forceModel_.srp_.shadowModel_ = EShadowModel::eCylindrical;
        } else if(aEqualsIgnoreCase(value, "None")){
            forceModel_.srp_.shadowModel_ = EShadowModel::eNone;
        }else{
            // @todo 处理其他阴影模型
        }
    }
    else if(aEqualsIgnoreCase(key, "EclipsingBodies")){
        // @todo 处理EclipsingBodies
    }
    else if(aEqualsIgnoreCase(key, "AtmAltForEclipse")){
        forceModel_.srp_.atmAltForEclipse_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "SRPModel")){
        // @todo 处理SRPModel
    }
    else if(aEqualsIgnoreCase(key, "IncludeGenRelCorrection")){
        // @todo 处理IncludeGenRelCorrection
    }
    else if(aEqualsIgnoreCase(key, "IncludeAlbedo")){
        // @todo 处理IncludeAlbedo
    }
    else if(aEqualsIgnoreCase(key, "IncludeThermalRadPressure")){
        // @todo 处理IncludeThermalRadPressure
    }
    else if(aEqualsIgnoreCase(key, "Include2ndOrderStateEstDeriv")){
        // @todo 处理Include2ndOrderStateEstDeriv
    }
    else if(aEqualsIgnoreCase(key, "RadPressureCoefficient")){
        forceModel_.srp_.coefficient_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "RadPressureArea")){
        // @todo 处理RadPressureArea
    }
    else if(aEqualsIgnoreCase(key, "GroundReflectionModelFile")){
        // @todo 处理GroundReflectionModelFile
    }
    else if(aEqualsIgnoreCase(key, "ThirdBodyGravity")){
        // 处理三体引力
        auto body = aGetBody(value);
        if(body){
            HPOPForceModel::ThirdBody thirdBody;
            thirdBody.body_ = body;
            forceModel_.thirdBodies_.push_back(thirdBody);
        }
    }
    else if(aEqualsIgnoreCase(key, "EphRefFrame")){
        // @todo 处理EphRefFrame
    }
    // 下面是积分器的相关参数
    else if(aEqualsIgnoreCase(key, "IntegMethod")){
        // @todo 处理IntegMethod
    }
    else if(aEqualsIgnoreCase(key, "UseVOP")){
        // @todo 处理UseVOP
    }
    else if(aEqualsIgnoreCase(key, "PseudoCorrect")){
        // @todo 处理PseudoCorrect
    }
    else if(aEqualsIgnoreCase(key, "UseRegTime")){
        // @todo 处理UseRegTime
    }
    else if(aEqualsIgnoreCase(key, "StepControlMethod")){
        // @todo 处理StepControlMethod
    }
    else if(aEqualsIgnoreCase(key, "ErrorTolerance")){
        // @todo 处理ErrorTolerance
    }
    else if(aEqualsIgnoreCase(key, "TimeStep")){
        // @todo 处理TimeStep
    }
    else if(aEqualsIgnoreCase(key, "MinStepSize")){
        // @todo 处理MinStepSize
    }
    else if(aEqualsIgnoreCase(key, "MaxStepSize")){
        // @todo 处理MaxStepSize
    }
    else if(aEqualsIgnoreCase(key, "UseFixedAnomPredict")){
        // @todo 处理UseFixedAnomPredict
    }
    else if(aEqualsIgnoreCase(key, "FixedAnomalyStep")){
        // @todo 处理FixedAnomalyStep
    }
    else if(aEqualsIgnoreCase(key, "FixedAnomEccCutOff")){
        // @todo 处理FixedAnomEccCutOff
    }
    else if(aEqualsIgnoreCase(key, "ReportOnFixedStep")){
        // @todo 处理ReportOnFixedStep
    }
    else if(aEqualsIgnoreCase(key, "ReportKp")){
        // @todo 处理ReportKp
    }
    else if(aEqualsIgnoreCase(key, "InterpolationSamplesM1")){
        // @todo 处理InterpolationSamplesM1
    }
    else if(aEqualsIgnoreCase(key, "InterpolationMethod")){
        // @todo 处理InterpolationMethod
    }
    else if(aEqualsIgnoreCase(key, "AltitudeCutOff")){
        // @todo 处理AltitudeCutOff
    }
    else if(aEqualsIgnoreCase(key, "UsePlugin")){
        // @todo 处理UsePlugin
    }
    return MotionBasicSax::keyValue(key, value);
}

err_t MotionHPOPSax::getMotion(ScopedPtr<MotionProfile> &motion)
{
    PMotionHPOP motionHPOP = MotionHPOP::New();
    if(!motionHPOP){
        return eErrorInvalidParam;
    }
    
    // 设置力模型
    motionHPOP->setForceModel(forceModel_);
    
    // 设置初始状态
    auto cartState = StateCartesian::New();
    cartState->setState(cartState_);
    cartState->setStateEpoch(makeStateEpoch());

    motionHPOP->setInitialState(cartState);
    motionHPOP->setInterval(makeInterval());
    
    motion = motionHPOP;
    return eNoError;
}

AST_NAMESPACE_END