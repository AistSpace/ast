///
/// @file      SensorLoader.cpp
/// @brief     传感器加载器实现
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

#include "SensorLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Sensor.hpp"
#include "AstSim/CentroidPosition.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载传感器图案信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorPattern(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "DetailedTerrainHorizon")){
                // @todo 设置详细地形地平线
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "SimpleCone")){
                BKVItemView coneItem;
                BKVParser::EToken coneToken;
                do{
                    coneToken = parser.getNext(coneItem);
                    if(coneToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(coneItem.key(), "ConeAngle")){
                            // @todo 设置锥角
                        }else if(aEqualsIgnoreCase(coneItem.key(), "AngularPatternResolution")){
                            // @todo 设置角分辨率
                        }
                    }else if(coneToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(coneItem.value(), "SimpleCone")){
                            break;
                        }
                    }
                }while(coneToken != BKVParser::eEOF);
            }else if(aEqualsIgnoreCase(item.value(), "Conical")){
                BKVItemView coneItem;
                BKVParser::EToken coneToken;
                do{
                    coneToken = parser.getNext(coneItem);
                    if(coneToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(coneItem.key(), "InnerConeAngle")){
                            // @todo 设置内锥角
                        }else if(aEqualsIgnoreCase(coneItem.key(), "OuterConeAngle")){
                            // @todo 设置外锥角
                        }else if(aEqualsIgnoreCase(coneItem.key(), "MinClockAngle")){
                            // @todo 设置最小时钟角
                        }else if(aEqualsIgnoreCase(coneItem.key(), "MaxClockAngle")){
                            // @todo 设置最大时钟角
                        }
                    }else if(coneToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(coneItem.value(), "Conical")){
                            break;
                        }
                    }
                }while(coneToken != BKVParser::eEOF);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器指向信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorPointing(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Fixed")){
                BKVItemView fixedItem;
                BKVParser::EToken fixedToken;
                do{
                    fixedToken = parser.getNext(fixedItem);
                    if(fixedToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(fixedItem.key(), "PointingMethod")){
                            // @todo 设置指向方法
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Sequence")){
                            // @todo 设置序列
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qx")){
                            // @todo 设置四元数x
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qy")){
                            // @todo 设置四元数y
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qz")){
                            // @todo 设置四元数z
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qs")){
                            // @todo 设置四元数s
                        }
                    }else if(fixedToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(fixedItem.value(), "Fixed")){
                            break;
                        }
                    }
                }while(fixedToken != BKVParser::eEOF);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pointing")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器光学信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorOptics(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "FocalLength")){
                // @todo 设置焦距
            }else if(aEqualsIgnoreCase(item.key(), "DetectorPitch")){
                // @todo 设置探测器间距
            }else if(aEqualsIgnoreCase(item.key(), "IQConstant")){
                // @todo 设置IQ常数
            }else if(aEqualsIgnoreCase(item.key(), "GsdCoefficient")){
                // @todo 设置GSD系数
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Optics")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器位置信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorLocation(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Location")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器扩展信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorExtensions(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ExternData")){
                // @todo 处理外部数据
            }else if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
                // @todo 处理ADF文件数据
            }else if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                // @todo 处理访问约束
            }else if(aEqualsIgnoreCase(item.value(), "ObjectCoverage")){
                // @todo 处理对象覆盖
            }else if(aEqualsIgnoreCase(item.value(), "Desc")){
                // @todo 处理描述
            }else if(aEqualsIgnoreCase(item.value(), "Refraction")){
                // @todo 处理折射
            }else if(aEqualsIgnoreCase(item.value(), "Crdn")){
                // @todo 处理坐标系统
            }else if(aEqualsIgnoreCase(item.value(), "Graphics")){
                // @todo 处理图形
            }else if(aEqualsIgnoreCase(item.value(), "Swath")){
                // @todo 处理扫描带
            }else if(aEqualsIgnoreCase(item.value(), "VO")){
                // @todo 处理VO
            }else if(aEqualsIgnoreCase(item.value(), "DIS")){
                // @todo 处理DIS
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


/// @brief 加载传感器对象
/// @param parser BKV解析器
/// @param sensorType 传感器类型
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(BKVParser &parser, StringView sensorType, Sensor &sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Name")){
                sensor.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                if(errc_t rc = _aLoadSensorPattern(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Location")){
                if(errc_t rc = _aLoadSensorLocation(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Pointing")){
                if(errc_t rc = _aLoadSensorPointing(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Optics")){
                if(errc_t rc = _aLoadSensorOptics(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadSensorExtensions(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &sensor)){
                    return rc;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), sensorType)){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器对象
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(BKVParser& parser, Sensor& sensor)
{
    return aLoadSensor(parser, "Sensor", sensor);
}

/// @brief 加载传感器对象
/// @param filepath 文件路径
/// @param sensorType 传感器类型
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(StringView filepath, StringView sensorType, Sensor& sensor)
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
            // 第一个BEGIN END块的名称就是对象的类型
            StringView objectType = item.value();
            return aLoadSensor(parser, objectType, sensor);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}

/// @brief 加载传感器对象
/// @param filepath 文件路径
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(StringView filepath, Sensor& sensor)
{
    return aLoadSensor(filepath, "Sensor", sensor);
}

AST_NAMESPACE_END