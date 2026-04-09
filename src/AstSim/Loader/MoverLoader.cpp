///
/// @file      MoverLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "MoverLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "MotionTwoBodySax.hpp"
#include "MotionHPOPSax.hpp"
#include "AstCore/STKEphemerisFileParser.hpp"

AST_NAMESPACE_BEGIN




errc_t _aLoadTwoBody(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionTwoBodySax sax(parser, vehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadHPOP(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionHPOPSax sax(parser, vehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadSPICE(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        std::string filename_;
        int satelliteID_{-1};
        int segmentNumber_{1};
        double stepSize_{60.0};
        bool useLTDelay_{false};
        bool use1stOrderDelay_{false};
        bool use3rdOrderDelay_{false};
        TimePoint startTime_{};
        TimePoint stopTime_{};
        SharedPtr<EventInterval> ephemSmartInterval_;
    } data;


    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "FileName")){
                data.filename_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "SatelliteID")){
                data.satelliteID_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "SegmentNumber")){
                data.segmentNumber_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "StepSize")){
                data.stepSize_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "UseLTDelay")){
                data.useLTDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "Use1stOrderDelay")){
                data.use1stOrderDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "Use3rdOrderDelay")){
                data.use3rdOrderDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "StartTime")){
                data.startTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                data.stopTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "EphemSmartInterval")){
                errc_t rc = _aLoadEventInterval(parser, data.ephemSmartInterval_);
                if(rc)
                    aError("failed to load EphemSmartInterval");
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                // 处理事件间隔
                while(1){
                    BKVItemView eventItem;
                    BKVParser::EToken eventToken;
                    eventToken = parser.getNext(eventItem);
                    if(eventToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(eventItem.value(), "EVENTINTERVAL")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SPICE")){
                break;
            }
        }else if(token == BKVParser::eEOF){
            return eNoError;
        }else{
            return eErrorInvalidFile;
        }
    }

    // 创建 SPICE 运动模型
    auto motionSPICE = MotionSPICE::New();
    
    // 设置 SPICE 文件路径
    motionSPICE->setSpiceFile(data.filename_);
    motionSPICE->setSpiceIndex(data.satelliteID_);
    
    // 设置步长
    motionSPICE->setStepSize(data.stepSize_);
    
    // 设置时间间隔
    auto explicitInterval = EventIntervalExplicit::New(data.startTime_, data.stopTime_);
    auto fallbackInterval = EventIntervalFallback::New(data.ephemSmartInterval_, explicitInterval);
    motionSPICE->setInterval(fallbackInterval);

    motionProfile = motionSPICE;
    
    return eNoError;
}

errc_t _aLoadPassDefn(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Break")){
                // @todo 处理分割点
            }else if(aEqualsIgnoreCase(item.key(), "CoordSystem")){
                // @todo 处理坐标系
            }else if(aEqualsIgnoreCase(item.key(), "BreakType")){
                // @todo 处理分割类型
            }else if(aEqualsIgnoreCase(item.key(), "BreakAngle")){
                // @todo 处理分割角度
            }else if(aEqualsIgnoreCase(item.key(), "PartialPassMethod")){
                // @todo 这是什么???
            }else if(aEqualsIgnoreCase(item.key(), "DisplayFlag")){
                // @todo 处理显示标志
            }else if(aEqualsIgnoreCase(item.key(), "FirstPass")){
                // @todo 处理第一次通过
            }else if(aEqualsIgnoreCase(item.key(), "FirstPath")){
                // @todo 处理第一个路径
            }else if(aEqualsIgnoreCase(item.key(), "RepeatCycle")){
                // @todo 处理重复周期
            }else if(aEqualsIgnoreCase(item.key(), "MaintainPassNum")){
                // @todo 处理保持通过次数
            }else if(aEqualsIgnoreCase(item.key(), "RangeFirstPass")){
                // @todo 处理第一次通过
            }else if(aEqualsIgnoreCase(item.key(), "RangeLastPass")){
                // @todo 处理最后一次通过
            }else if(aEqualsIgnoreCase(item.key(), "DisplayScheme")){
                // @todo 处理显示方案
            }else if(aEqualsIgnoreCase(item.key(), "ScenarioEpoch")){
                // @todo 处理场景历元
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "PassDefn")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadVehiclePath(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    VehiclePathData data;
    data.centralBody_ = aGetDefaultBody();  // default body
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                data.centralBody_ = aGetBody(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StoreEphemeris")){
                data.storeEphemeris_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "SmoothInterp")){
                data.smoothInterp_ = item.value().toBool();
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "TwoBody")){
                if(errc_t rc = _aLoadTwoBody(parser, data, mover.getMotionProfileHandle())){
                    return rc;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "HPOP"))
            {
                if(errc_t rc = _aLoadHPOP(parser, data, mover.getMotionProfileHandle())){
                    return rc;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "SPICE")){
                if(errc_t rc = _aLoadSPICE(parser, data, mover.getMotionProfileHandle())){
                    return rc;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "PassDefn")){
                if(errc_t rc = _aLoadPassDefn(parser, mover)){
                    return rc;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "VehiclePath")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


errc_t _aLoadEphemeris(BKVParser& parser, Mover& mover)
{
    errc_t rc = aLoadSTKEphemeris(parser, mover.getEphemerisHandle());
    return rc;
}

errc_t _aLoadMassProperties(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Mass")){
                // @todo 处理质量
            }else if(aEqualsIgnoreCase(item.key(), "InertiaXX")){
                // @todo 处理转动惯量XX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaYX")){
                // @todo 处理转动惯量YX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaYY")){
                // @todo 处理转动惯量YY
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZX")){
                // @todo 处理转动惯量ZX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZY")){
                // @todo 处理转动惯量ZY
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZZ")){
                // @todo 处理转动惯量ZZ
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "MassProperties")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadAttitude(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "ScenarioEpoch")){
                // @todo 处理场景历元
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Profile")){
                // @todo 处理姿态定义
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Attitude")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadSwath(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "SwathType")){
                // @todo 处理扫描带类型
            }else if(aEqualsIgnoreCase(item.key(), "ElevationAngle")){
                // @todo 处理仰角
            }else if(aEqualsIgnoreCase(item.key(), "HalfAngle")){
                // @todo 处理半角
            }else if(aEqualsIgnoreCase(item.key(), "Distance")){
                // @todo 处理距离
            }else if(aEqualsIgnoreCase(item.key(), "RepType")){
                // @todo 处理表示类型
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Swath")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadEclipse(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Sunlight")){
                // @todo 处理阳光
            }else if(aEqualsIgnoreCase(item.key(), "SunlightLineStyle")){
                // @todo 处理阳光线样式
            }else if(aEqualsIgnoreCase(item.key(), "SunlightLineWidth")){
                // @todo 处理阳光线宽度
            }else if(aEqualsIgnoreCase(item.key(), "SunlightMarkerStyle")){
                // @todo 处理阳光标记样式
            }else if(aEqualsIgnoreCase(item.key(), "Penumbra")){
                // @todo 处理半影
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraLineStyle")){
                // @todo 处理半影线样式
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraLineWidth")){
                // @todo 处理半影线宽度
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraMarkerStyle")){
                // @todo 处理半影标记样式
            }else if(aEqualsIgnoreCase(item.key(), "Umbra")){
                // @todo 处理本影
            }else if(aEqualsIgnoreCase(item.key(), "UmbraLineStyle")){
                // @todo 处理本影线样式
            }else if(aEqualsIgnoreCase(item.key(), "UmbraLineWidth")){
                // @todo 处理本影线宽度
            }else if(aEqualsIgnoreCase(item.key(), "UmbraMarkerStyle")){
                // @todo 处理本影标记样式
            }else if(aEqualsIgnoreCase(item.key(), "SunlightPenumbraLine")){
                // @todo 处理阳光半影线
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraUmbraLine")){
                // @todo 处理半影本影线
            }else if(aEqualsIgnoreCase(item.key(), "SunlightColor")){
                // @todo 处理阳光颜色
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraColor")){
                // @todo 处理半影颜色
            }else if(aEqualsIgnoreCase(item.key(), "UmbraColor")){
                // @todo 处理本影颜色
            }else if(aEqualsIgnoreCase(item.key(), "UseCustomEclipseBodies")){
                // @todo 处理是否使用自定义遮挡天体??
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Eclipse")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadRealTimeDef(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "HistoryDuration")){
                // @todo 处理历史持续时间
            }else if(aEqualsIgnoreCase(item.key(), "LookAheadDuration")){
                // @todo 处理前瞻持续时间
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "RealTimeDef")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            // @todo 处理各种扩展块
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadMover(BKVParser& parser, StringView moverType, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Name")){
                mover.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "VehiclePath")){
                if(errc_t rc = _aLoadVehiclePath(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Ephemeris")){
                if(errc_t rc = _aLoadEphemeris(parser, mover)){
                    return rc;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "MassProperties")){
                if(errc_t rc = _aLoadMassProperties(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Attitude")){
                if(errc_t rc = _aLoadAttitude(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Swath")){
                if(errc_t rc = _aLoadSwath(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Eclipse")){
                if(errc_t rc = _aLoadEclipse(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "RealTimeDef")){
                if(errc_t rc = _aLoadRealTimeDef(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadExtensions(parser, mover)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &mover)){
                    return rc;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), moverType)){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


errc_t _aLoadSatellite(BKVParser& parser, Mover& mover)
{
    return _aLoadMover(parser, "Satellite", mover);
}


errc_t aLoadMover(StringView filepath, Mover &mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        aError("failed to open file '%.*s'", (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            // 处理文件开头的版本信息和作者信息
            // @todo 处理版本信息
        }
        else if(token == BKVParser::eBlockBegin){
            // if(aEqualsIgnoreCase(item.value(), "Satellite")){
            //     if(errc_t rc = _aLoadSatellite(parser, mover)){
            //         return rc;
            //     }
            // }

            
            // 第一个BEGIN END块的名称就是Mover的类型
            StringView moverType = item.value();
            return _aLoadMover(parser, moverType, mover);
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


AST_NAMESPACE_END