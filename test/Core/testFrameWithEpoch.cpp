///
/// @file      testFrameWithEpoch.cpp
/// @brief     带轴系历元的坐标系测试
/// @details   测试FrameWithEpoch类的创建、轴系历元管理和坐标转换
/// @author    axel
/// @date      2026-04-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见： 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstCore/Coordinate/Frame/FrameWithEpoch.hpp"
#include "AstCore/Point/PlanetPoint.hpp"
#include "AstCore/Axes/InertialAxes.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

class FrameWithEpochTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

TEST_F(FrameWithEpochTest, DefaultConstruction)
{
    FrameWithEpoch frame;
    
    // 默认构造函数：useEpoch_=false, sourceAxes_=nullptr
    // 所以 getAxes() 返回 sourceAxes_.get() = nullptr
    EXPECT_EQ(frame.getAxes(), nullptr);
    EXPECT_EQ(frame.getOrigin(), nullptr);
    EXPECT_FALSE(frame.getUseEpoch());
}

TEST_F(FrameWithEpochTest, NewConstruction)
{
    // 创建原点和轴系
    PlanetPoint origin;
    origin.setName("Earth");
    InertialAxes axes;
    
    // 无历元的构造
    {
        auto frame = FrameWithEpoch::New(&origin, &axes);
        EXPECT_NE(frame, nullptr);
        EXPECT_EQ(frame->getOrigin(), &origin);
        EXPECT_EQ(frame->getSourceAxes(), &axes);
        EXPECT_FALSE(frame->getUseEpoch());
        
        delete frame;
    }
    
    // 带历元的构造
    {
        EventTime epoch;
        auto frame = FrameWithEpoch::New(&origin, &axes, &epoch);
        EXPECT_NE(frame, nullptr);
        EXPECT_TRUE(frame->getUseEpoch());
        
        delete frame;
    }
}

TEST_F(FrameWithEpochTest, MakeShared)
{
    PlanetPoint origin;
    origin.setName("Earth");
    InertialAxes axes;
    
    // 无历元的MakeShared
    {
        auto frame = FrameWithEpoch::MakeShared(&origin, &axes);
        EXPECT_NE(frame, nullptr);
        EXPECT_EQ(frame->getOrigin(), &origin);
        EXPECT_EQ(frame->getSourceAxes(), &axes);
    }
    
    // 带历元的MakeShared
    {
        EventTime epoch;
        auto frame = FrameWithEpoch::MakeShared(&origin, &axes, &epoch);
        EXPECT_NE(frame, nullptr);
        EXPECT_TRUE(frame->getUseEpoch());
    }
}

TEST_F(FrameWithEpochTest, EpochManagement)
{
    PlanetPoint origin;
    InertialAxes axes;
    EventTime epoch1, epoch2;
    
    auto frame = FrameWithEpoch::New(&origin, &axes);
    
    // 默认不使用epoch
    EXPECT_FALSE(frame->getUseEpoch());
    
    // 设置epoch
    frame->setEpoch(&epoch1);
    EXPECT_TRUE(frame->getUseEpoch());
    EXPECT_EQ(frame->getEpoch(), &epoch1);
    
    // 切换epoch
    frame->setEpoch(&epoch2);
    EXPECT_EQ(frame->getEpoch(), &epoch2);
    
    // 获取epoch handle
    auto& epochHandle = frame->getEpochHandle();
    EXPECT_NE(epochHandle, nullptr);
    
    // 获取epoch为TimePoint
    TimePoint tp;
    frame->getEpoch(tp);
    
    delete frame;
}

TEST_F(FrameWithEpochTest, AxesManagement)
{
    PlanetPoint origin;
    InertialAxes axes1, axes2;
    EventTime epoch;
    
    auto frame = FrameWithEpoch::New(&origin, &axes1, &epoch);
    
    // 获取source axes
    EXPECT_EQ(frame->getSourceAxes(), &axes1);
    
    // 更改source axes
    frame->setSourceAxes(&axes2);
    EXPECT_EQ(frame->getSourceAxes(), &axes2);
    
    // 获取reference axes
    EXPECT_NE(frame->getReferenceAxes(), nullptr);
    
    // 设置reference axes
    frame->setReferenceAxes(&axes1);
    EXPECT_EQ(frame->getReferenceAxes(), &axes1);
    
    delete frame;
}

TEST_F(FrameWithEpochTest, UseEpochToggle)
{
    PlanetPoint origin;
    InertialAxes axes;
    EventTime epoch;
    
    auto frame = FrameWithEpoch::New(&origin, &axes, &epoch);
    
    // 初始使用epoch
    EXPECT_TRUE(frame->getUseEpoch());
    EXPECT_EQ(frame->getAxes(), frame->getReferenceAxes());
    
    // 关闭epoch使用
    frame->setUseEpoch(false);
    EXPECT_FALSE(frame->getUseEpoch());
    EXPECT_EQ(frame->getAxes(), frame->getSourceAxes());
    
    // 重新开启epoch使用
    frame->setUseEpoch(true);
    EXPECT_TRUE(frame->getUseEpoch());
    
    delete frame;
}

TEST_F(FrameWithEpochTest, GetOrigin)
{
    PlanetPoint origin;
    origin.setName("Earth");
    InertialAxes axes;
    
    auto frame = FrameWithEpoch::New(&origin, &axes);
    EXPECT_EQ(frame->getOrigin(), &origin);
    
    delete frame;
}

TEST_F(FrameWithEpochTest, GetAxes)
{
    PlanetPoint origin;
    InertialAxes axes;
    EventTime epoch;
    
    // 不使用epoch时，返回source axes
    {
        auto frame = FrameWithEpoch::New(&origin, &axes);
        frame->setUseEpoch(false);
        EXPECT_EQ(frame->getAxes(), &axes);
        delete frame;
    }
    
    // 使用epoch时，返回frozen axes
    {
        auto frame = FrameWithEpoch::New(&origin, &axes, &epoch);
        EXPECT_TRUE(frame->getUseEpoch());
        EXPECT_NE(frame->getAxes(), &axes);
        delete frame;
    }
}
