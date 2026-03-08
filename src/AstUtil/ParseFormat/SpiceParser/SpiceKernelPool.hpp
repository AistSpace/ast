///
/// @file      SpiceKernelPool.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-08
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#pragma once

#include "AstGlobal.h"
#include <unordered_map>
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class SpiceData
{
public:
    SpiceData(){
        new (&doubleData_) std::vector<double>();
    }
    SpiceData(const std::vector<double>& doubleData)
    {
        new (&doubleData_) std::vector<double>(doubleData);
    }
    SpiceData(const std::vector<int>& intData)
    {
        new (&intData_) std::vector<int>(intData);
    }
    SpiceData(const std::vector<char>& charData)
    {
        new (&charData_) std::vector<char>(charData);
    }
    SpiceData(const SpiceData& other)
    {
        new (&doubleData_) std::vector<double>(other.doubleData_);
    }
    SpiceData(SpiceData&& other)
    {
        new (&doubleData_) std::vector<double>(std::move(other.doubleData_));
    }
    SpiceData& operator=(const SpiceData& other)
    {
        doubleData_ = other.doubleData_;
        return *this;
    }
    SpiceData& operator=(SpiceData&& other)
    {
        doubleData_ = std::move(other.doubleData_);
        return *this;
    }
    ~SpiceData()
    {
        doubleData_.~vector<double>();
    }
    const std::vector<double>& getDoubleData() const { return doubleData_; }
    const std::vector<int>& getIntData() const { return intData_; }
    const std::vector<char>& getCharData() const { return charData_; }

    enum DataType
    {
        eDouble = 0,
        eInt = 1,
        eChar = 2,
    };
    // DataType type_;
    union{
        std::vector<double> doubleData_;
        std::vector<int>    intData_;
        std::vector<char>   charData_;
    };
};


/// @brief  SPICE 内核数据池
/// @details 该类用于管理从 SPICE 内核文件加载的数据
class AST_UTIL_API SpiceKernelPool
{
public:
    using DataMap = std::unordered_map<std::string, SpiceData>;
    SpiceKernelPool() = default;
    ~SpiceKernelPool() = default;

    /// @brief 设置数据
    /// @param name 数据名称
    /// @param data 数据
    void setData(StringView name, const SpiceData& data);

    /// @brief 设置双精度数据
    /// @param name 数据名称
    /// @param data 数据
    void setDoubleData(StringView name, const std::vector<double>& data);

    /// @brief 设置整数数据
    /// @param name 数据名称
    /// @param data 数据
    void setIntData(StringView name, const std::vector<int>& data);
    
    /// @brief 设置字符数据
    /// @param name 数据名称
    /// @param data 数据
    void setCharData(StringView name, const std::vector<char>& data);


    /// @brief 获取数据
    /// @param name 数据名称
    /// @return 指向数据的指针，若数据不存在则返回 nullptr
    const SpiceData* getData(StringView name) const;

    /// @brief 获取双精度数据
    /// @param name 数据名称
    /// @return 指向双精度数据的指针，若数据不存在则返回 nullptr
    const std::vector<double>* getDoubleData(StringView name) const;
    
    /// @brief 获取整数数据
    /// @param name 数据名称
    /// @return 指向整数数据的指针，若数据不存在则返回 nullptr
    const std::vector<int>* getIntData(StringView name) const;
    
    /// @brief 获取字符数据
    /// @param name 数据名称
    /// @return 指向字符数据的指针，若数据不存在则返回 nullptr
    const std::vector<char>* getCharData(StringView name) const;

    DataMap::iterator begin() { return dataMap_.begin(); }
    DataMap::iterator end() { return dataMap_.end(); }
    DataMap::const_iterator begin() const { return dataMap_.begin(); }
    DataMap::const_iterator end() const { return dataMap_.end(); }
    size_t size() const { return dataMap_.size(); }
protected:
    DataMap dataMap_;
};

/*! @} */

AST_NAMESPACE_END
