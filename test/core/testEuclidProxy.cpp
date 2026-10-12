///
/// @file      testEuclidProxy.cpp
/// @brief     Euclid 模块代理类单元测试
/// @details   测试 AxesProxy、FrameProxy、PointProxy、VectorProxy、AngleProxy
///            的空实现回退与接口转发行为。
/// @author    axel
/// @date      2026-10-12
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Axes.hpp"
#include "ast/Frame.hpp"
#include "ast/Point.hpp"
#include "ast/Vector.hpp"
#include "ast/Angle.hpp"
#include "ast/AxesProxy.hpp"
#include "ast/FrameProxy.hpp"
#include "ast/PointProxy.hpp"
#include "ast/VectorProxy.hpp"
#include "ast/AngleProxy.hpp"
#include "ast/Transform.hpp"
#include "ast/KinematicTransform.hpp"
#include "ast/AccelerationTransform.hpp"
#include "ast/Rotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/AccelerationRotation.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Matrix.hpp"
#include "ast/RunTime.hpp"
#include "ast/AstTestMacro.h"

namespace
{

AST_USING_NAMESPACE

// ============================================
// Test Fixture
// ============================================
class EuclidProxyTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        aInitialize();
    }

    static TimePoint TestTime()
    {
        return TimePoint::FromUTC(2026, 10, 12, 0, 0, 0);
    }
};

/*!
    @note 代理类以 WeakPtr 持有被代理对象，而弱引用参与对象的生命周期管理，
          仅对堆对象成立。因此下面所有测试替身一律用 new 创建、并由 SharedPtr
          保活，不得使用栈对象。
    @note 各测试替身返回 eErrorNotImplemented，与应用了空实现回退的
          eErrorNullPtr 相区分，以确认调用确实转发到了被代理对象。
*/

// ============================================
// 测试替身
// ============================================

/// @brief 测试用坐标系：各接口返回可辨识的结果并记录调用次数
class TestProxyFrame : public Frame
{
public:
    Axes* getAxes() const override { ++axesCalls_; return axes_; }
    Point* getOrigin() const override { ++originCalls_; return origin_; }
    Frame* getParent() const override { ++parentCalls_; return parent_; }

    errc_t getTransform(const TimePoint&, Transform&) const override
    {
        ++transformCalls_;
        return eErrorNotImplemented;
    }
    errc_t getTransform(const TimePoint&, KinematicTransform&) const override
    {
        ++kinTransformCalls_;
        return eErrorNotImplemented;
    }
    errc_t getTransform(const TimePoint&, AccelerationTransform&) const override
    {
        ++accTransformCalls_;
        return eErrorNotImplemented;
    }

    Frame* parent_{nullptr};
    Axes* axes_{nullptr};
    Point* origin_{nullptr};

    mutable int axesCalls_{0};
    mutable int originCalls_{0};
    mutable int parentCalls_{0};
    mutable int transformCalls_{0};
    mutable int kinTransformCalls_{0};
    mutable int accTransformCalls_{0};
};

/// @brief 测试用点：各接口返回可辨识的结果并记录调用次数
class TestProxyPoint : public Point
{
public:
    Frame* getFrame() const override { ++frameCalls_; return frame_; }

    errc_t getPos(const TimePoint&, Vector3d& pos) const override
    {
        ++posCalls_;
        pos = pos_;
        return eErrorNotImplemented;
    }
    errc_t getPosVel(const TimePoint&, Vector3d&, Vector3d&) const override
    {
        ++posVelCalls_;
        return eErrorNotImplemented;
    }
    errc_t getPosVelAcc(const TimePoint&, Vector3d&, Vector3d&, Vector3d&) const override
    {
        ++posVelAccCalls_;
        return eErrorNotImplemented;
    }
    errc_t getInterval(TimeInterval&) const override
    {
        ++intervalCalls_;
        return eErrorNotImplemented;
    }

    Frame* frame_{nullptr};
    Vector3d pos_{1.0, 2.0, 3.0};

    mutable int frameCalls_{0};
    mutable int posCalls_{0};
    mutable int posVelCalls_{0};
    mutable int posVelAccCalls_{0};
    mutable int intervalCalls_{0};
};

/// @brief 测试用轴系：各接口返回可辨识的结果并记录调用次数
class TestProxyAxes : public Axes
{
public:
    Axes* getParent() const override { ++parentCalls_; return parent_; }

    errc_t getTransform(const TimePoint&, Rotation&) const override
    {
        ++transformCalls_;
        return eErrorNotImplemented;
    }
    errc_t getTransform(const TimePoint&, KinematicRotation&) const override
    {
        ++kinTransformCalls_;
        return eErrorNotImplemented;
    }
    errc_t getTransform(const TimePoint&, AccelerationRotation&) const override
    {
        ++accTransformCalls_;
        return eErrorNotImplemented;
    }

    Axes* parent_{nullptr};

    mutable int parentCalls_{0};
    mutable int transformCalls_{0};
    mutable int kinTransformCalls_{0};
    mutable int accTransformCalls_{0};
};

/// @brief 测试用向量：各接口返回可辨识的结果并记录调用次数
class TestProxyVector : public Vector
{
public:
    using Vector::getVector;   // 引入加速度版本的默认实现

    Axes* getAxes() const override { ++axesCalls_; return axes_; }

    errc_t getVector(const TimePoint&, Vector3d&) const override
    {
        ++vectorCalls_;
        return eErrorNotImplemented;
    }
    errc_t getVector(const TimePoint&, Vector3d&, Vector3d&) const override
    {
        ++vectorVelCalls_;
        return eErrorNotImplemented;
    }
    errc_t getVector(const TimePoint&, Vector3d&, Vector3d&, Vector3d&) const override
    {
        ++vectorVelAccCalls_;
        return eErrorNotImplemented;
    }

    Axes* axes_{nullptr};

    mutable int axesCalls_{0};
    mutable int vectorCalls_{0};
    mutable int vectorVelCalls_{0};
    mutable int vectorVelAccCalls_{0};
};

/// @brief 测试用角度：各接口返回可辨识的结果并记录调用次数
class TestProxyAngle : public Angle
{
public:
    errc_t getAngle(const TimePoint&, double& value) const override
    {
        ++angleCalls_;
        value = angle_;
        return eErrorNotImplemented;
    }
    errc_t getAngle(const TimePoint&, double& value, double&) const override
    {
        ++angleVelCalls_;
        value = angle_;
        return eErrorNotImplemented;
    }

    double angle_{1.25};

    mutable int angleCalls_{0};
    mutable int angleVelCalls_{0};
};

// ============================================
// 空实现回退：未设置被代理对象时的行为
// ============================================

TEST_F(EuclidProxyTest, FrameProxyNullImpl)
{
    const TimePoint tp = TestTime();
    FrameProxy proxy;

    EXPECT_EQ(proxy.impl(), nullptr);
    EXPECT_EQ(proxy.getParent(), nullptr);
    EXPECT_EQ(proxy.getAxes(), nullptr);
    EXPECT_EQ(proxy.getOrigin(), nullptr);

    Transform transform;
    KinematicTransform kinTransform;
    AccelerationTransform accTransform;
    EXPECT_EQ(proxy.getTransform(tp, transform), eErrorNullPtr);
    EXPECT_EQ(proxy.getTransform(tp, kinTransform), eErrorNullPtr);
    EXPECT_EQ(proxy.getTransform(tp, accTransform), eErrorNullPtr);

    // 空实现时回退到 Object::getRepresentation()，即代理对象自身的名称
    proxy.setName("ProxyOnly");
    EXPECT_EQ(proxy.getRepresentation(), "ProxyOnly");
}

TEST_F(EuclidProxyTest, PointProxyNullImpl)
{
    const TimePoint tp = TestTime();
    PointProxy proxy;

    EXPECT_EQ(proxy.impl(), nullptr);
    EXPECT_EQ(proxy.getFrame(), nullptr);

    Vector3d pos, vel, acc;
    TimeInterval interval;
    EXPECT_EQ(proxy.getPos(tp, pos), eErrorNullPtr);
    EXPECT_EQ(proxy.getPosVel(tp, pos, vel), eErrorNullPtr);
    EXPECT_EQ(proxy.getPosVelAcc(tp, pos, vel, acc), eErrorNullPtr);
    EXPECT_EQ(proxy.getInterval(interval), eErrorNullPtr);
}

TEST_F(EuclidProxyTest, VectorProxyNullImpl)
{
    const TimePoint tp = TestTime();
    VectorProxy proxy;

    EXPECT_EQ(proxy.impl(), nullptr);
    EXPECT_EQ(proxy.getAxes(), nullptr);

    Vector3d vec, vel, acc;
    EXPECT_EQ(proxy.getVector(tp, vec), eErrorNullPtr);
    EXPECT_EQ(proxy.getVector(tp, vec, vel), eErrorNullPtr);
    EXPECT_EQ(proxy.getVector(tp, vec, vel, acc), eErrorNullPtr);
}

TEST_F(EuclidProxyTest, AxesProxyNullImpl)
{
    const TimePoint tp = TestTime();
    AxesProxy proxy;

    EXPECT_EQ(proxy.impl(), nullptr);
    EXPECT_EQ(proxy.getParent(), nullptr);

    Rotation rotation;
    KinematicRotation kinRotation;
    AccelerationRotation accRotation;
    EXPECT_EQ(proxy.getTransform(tp, rotation), eErrorNullPtr);
    EXPECT_EQ(proxy.getTransform(tp, kinRotation), eErrorNullPtr);
    EXPECT_EQ(proxy.getTransform(tp, accRotation), eErrorNullPtr);
}

TEST_F(EuclidProxyTest, AngleProxyNullImpl)
{
    const TimePoint tp = TestTime();
    AngleProxy proxy;

    EXPECT_EQ(proxy.impl(), nullptr);

    double value = 0.0;
    double angVel = 0.0;
    EXPECT_EQ(proxy.getAngle(tp, value), eErrorNullPtr);
    EXPECT_EQ(proxy.getAngle(tp, value, angVel), eErrorNullPtr);
}

// ============================================
// 接口转发：设置被代理对象后，调用应原样传递给实现
// ============================================

TEST_F(EuclidProxyTest, FrameProxyForwarding)
{
    SharedPtr<TestProxyFrame> frame = new TestProxyFrame();
    SharedPtr<TestProxyAxes> axes = new TestProxyAxes();
    SharedPtr<TestProxyPoint> origin = new TestProxyPoint();
    SharedPtr<TestProxyFrame> parent = new TestProxyFrame();

    frame->setName("ImplFrame");
    frame->axes_ = axes.get();
    frame->origin_ = origin.get();
    frame->parent_ = parent.get();

    FrameProxy proxy(frame.get());
    const TimePoint tp = TestTime();

    EXPECT_EQ(proxy.impl(), frame.get());
    EXPECT_EQ(proxy.getAxes(), axes.get());
    EXPECT_EQ(proxy.getOrigin(), origin.get());
    EXPECT_EQ(proxy.getParent(), parent.get());
    EXPECT_EQ(frame->axesCalls_, 1);
    EXPECT_EQ(frame->originCalls_, 1);
    EXPECT_EQ(frame->parentCalls_, 1);

    Transform transform;
    KinematicTransform kinTransform;
    AccelerationTransform accTransform;
    EXPECT_EQ(proxy.getTransform(tp, transform), eErrorNotImplemented);
    EXPECT_EQ(proxy.getTransform(tp, kinTransform), eErrorNotImplemented);
    EXPECT_EQ(proxy.getTransform(tp, accTransform), eErrorNotImplemented);
    EXPECT_EQ(frame->transformCalls_, 1);
    EXPECT_EQ(frame->kinTransformCalls_, 1);
    EXPECT_EQ(frame->accTransformCalls_, 1);

    // 设置实现后，表示串转发给实现对象
    EXPECT_EQ(proxy.getRepresentation(), "ImplFrame");
}

TEST_F(EuclidProxyTest, FrameProxySetImpl)
{
    SharedPtr<TestProxyFrame> frame = new TestProxyFrame();

    FrameProxy proxy;
    proxy.setImpl(frame.get());
    EXPECT_EQ(proxy.impl(), frame.get());

    proxy.setImpl(nullptr);
    EXPECT_EQ(proxy.impl(), nullptr);
}

TEST_F(EuclidProxyTest, PointProxyForwarding)
{
    SharedPtr<TestProxyPoint> point = new TestProxyPoint();
    SharedPtr<TestProxyFrame> frame = new TestProxyFrame();
    point->frame_ = frame.get();

    PointProxy proxy(point.get());
    const TimePoint tp = TestTime();

    EXPECT_EQ(proxy.impl(), point.get());
    EXPECT_EQ(proxy.getFrame(), frame.get());
    EXPECT_EQ(point->frameCalls_, 1);

    Vector3d pos, vel, acc;
    TimeInterval interval;
    EXPECT_EQ(proxy.getPos(tp, pos), eErrorNotImplemented);
    EXPECT_NEAR(pos.x(), point->pos_.x(), 1e-15);
    EXPECT_NEAR(pos.y(), point->pos_.y(), 1e-15);
    EXPECT_NEAR(pos.z(), point->pos_.z(), 1e-15);
    EXPECT_EQ(point->posCalls_, 1);

    EXPECT_EQ(proxy.getPosVel(tp, pos, vel), eErrorNotImplemented);
    EXPECT_EQ(point->posVelCalls_, 1);

    EXPECT_EQ(proxy.getPosVelAcc(tp, pos, vel, acc), eErrorNotImplemented);
    EXPECT_EQ(point->posVelAccCalls_, 1);

    EXPECT_EQ(proxy.getInterval(interval), eErrorNotImplemented);
    EXPECT_EQ(point->intervalCalls_, 1);
}

TEST_F(EuclidProxyTest, VectorProxyForwarding)
{
    SharedPtr<TestProxyVector> vector = new TestProxyVector();
    SharedPtr<TestProxyAxes> axes = new TestProxyAxes();
    vector->axes_ = axes.get();

    VectorProxy proxy(vector.get());
    const TimePoint tp = TestTime();

    EXPECT_EQ(proxy.impl(), vector.get());
    EXPECT_EQ(proxy.getAxes(), axes.get());
    EXPECT_EQ(vector->axesCalls_, 1);

    Vector3d vec, vel, acc;
    EXPECT_EQ(proxy.getVector(tp, vec), eErrorNotImplemented);
    EXPECT_EQ(vector->vectorCalls_, 1);
    EXPECT_EQ(proxy.getVector(tp, vec, vel), eErrorNotImplemented);
    EXPECT_EQ(vector->vectorVelCalls_, 1);
    EXPECT_EQ(proxy.getVector(tp, vec, vel, acc), eErrorNotImplemented);
    EXPECT_EQ(vector->vectorVelAccCalls_, 1);
}

TEST_F(EuclidProxyTest, AxesProxyForwarding)
{
    SharedPtr<TestProxyAxes> axes = new TestProxyAxes();
    SharedPtr<TestProxyAxes> parent = new TestProxyAxes();
    axes->parent_ = parent.get();

    AxesProxy proxy(axes.get());
    const TimePoint tp = TestTime();

    EXPECT_EQ(proxy.impl(), axes.get());
    EXPECT_EQ(proxy.getParent(), parent.get());
    EXPECT_EQ(axes->parentCalls_, 1);

    Rotation rotation;
    KinematicRotation kinRotation;
    AccelerationRotation accRotation;
    EXPECT_EQ(proxy.getTransform(tp, rotation), eErrorNotImplemented);
    EXPECT_EQ(proxy.getTransform(tp, kinRotation), eErrorNotImplemented);
    EXPECT_EQ(proxy.getTransform(tp, accRotation), eErrorNotImplemented);
    EXPECT_EQ(axes->transformCalls_, 1);
    EXPECT_EQ(axes->kinTransformCalls_, 1);
    EXPECT_EQ(axes->accTransformCalls_, 1);
}

TEST_F(EuclidProxyTest, AngleProxyForwarding)
{
    SharedPtr<TestProxyAngle> angle = new TestProxyAngle();

    AngleProxy proxy(angle.get());
    const TimePoint tp = TestTime();

    EXPECT_EQ(proxy.impl(), angle.get());

    double value = 0.0;
    double angVel = 0.0;
    EXPECT_EQ(proxy.getAngle(tp, value), eErrorNotImplemented);
    EXPECT_NEAR(value, angle->angle_, 1e-15);
    EXPECT_EQ(angle->angleCalls_, 1);

    value = 0.0;
    EXPECT_EQ(proxy.getAngle(tp, value, angVel), eErrorNotImplemented);
    EXPECT_NEAR(value, angle->angle_, 1e-15);
    EXPECT_EQ(angle->angleVelCalls_, 1);
}

// ============================================
// 代理可挂载到父对象上（平台本体轴系的用法）
// ============================================

TEST_F(EuclidProxyTest, ProxyAsSharedObject)
{
    SharedPtr<TestProxyAxes> axes = new TestProxyAxes();

    // 代理常用于替换运行时对象，需支持共享引用计数管理
    HAxes handle = new AxesProxy(axes.get());
    ASSERT_TRUE(handle.get() != nullptr);
    EXPECT_EQ(handle->getParent(), nullptr);   // 转发给 TestProxyAxes，父系为空

    auto proxy = static_cast<AxesProxy*>(handle.get());
    EXPECT_EQ(proxy->impl(), axes.get());

    const TimePoint tp = TestTime();
    Rotation rotation;
    EXPECT_EQ(handle->getTransform(tp, rotation), eErrorNotImplemented);
    EXPECT_EQ(axes->transformCalls_, 1);
}

} // namespace

GTEST_MAIN()
