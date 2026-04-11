///
/// @file      testGeoCoordinate.cpp
/// @brief     GeoCoordinate模块测试
/// @details   测试地理坐标类的功能
/// @author    Aist
/// @date      2026-04-11
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

#include "AstCore/GeoCoordinate.hpp"
#include "AstMath/Vector.hpp"
#include "AstTest/Test.h"
#include <cmath>

AST_USING_NAMESPACE

// 角度转弧度辅助函数
constexpr double DEG2RAD(double deg) { return deg * M_PI / 180.0; }

// 测试 GeoCoordinate 构造函数
TEST(GeoCoordinateTest, Constructor)
{
    // 从纬经高构造（弧度）
    GeoCoordinate coord2(DEG2RAD(30.0), DEG2RAD(120.0), 100.0);
    EXPECT_NEAR(coord2.latitude(), DEG2RAD(30.0), 1e-14);
    EXPECT_NEAR(coord2.longitude(), DEG2RAD(120.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord2.altitude(), 100.0);
    
    // 零值构造
    GeoCoordinate coord3(0.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(coord3.latitude(), 0.0);
    EXPECT_DOUBLE_EQ(coord3.longitude(), 0.0);
    EXPECT_DOUBLE_EQ(coord3.altitude(), 0.0);
    
    // 南半球测试（负纬度）
    GeoCoordinate coord4(DEG2RAD(-45.0), DEG2RAD(-90.0), 200.0);
    EXPECT_NEAR(coord4.latitude(), DEG2RAD(-45.0), 1e-14);
    EXPECT_NEAR(coord4.longitude(), DEG2RAD(-90.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord4.altitude(), 200.0);
}

// 测试 setter 和 getter
TEST(GeoCoordinateTest, SettersAndGetters)
{
    GeoCoordinate coord;
    
    // 使用弧度值
    coord.setLatitude(DEG2RAD(35.0));
    EXPECT_NEAR(coord.latitude(), DEG2RAD(35.0), 1e-14);
    
    coord.setLongitude(DEG2RAD(116.0));
    EXPECT_NEAR(coord.longitude(), DEG2RAD(116.0), 1e-14);
    
    coord.setAltitude(50.0);
    EXPECT_DOUBLE_EQ(coord.altitude(), 50.0);

    // 测试引用返回的 setter（弧度）
    coord.latitude() = DEG2RAD(40.0);
    coord.longitude() = DEG2RAD(117.0);
    coord.altitude() = 60.0;
    EXPECT_NEAR(coord.latitude(), DEG2RAD(40.0), 1e-14);
    EXPECT_NEAR(coord.longitude(), DEG2RAD(117.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord.altitude(), 60.0);
    
    // 测试南半球
    coord.setLatitude(DEG2RAD(-30.0));
    EXPECT_NEAR(coord.latitude(), DEG2RAD(-30.0), 1e-14);
}

// 测试天顶方向向量
TEST(GeoCoordinateTest, GetZenith)
{
    // 赤道上
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 0.0, 1e-14);
    }
    
    // 北极
    {
        GeoCoordinate coord(DEG2RAD(90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 1.0, 1e-14);
    }

    // 南极
    {
        GeoCoordinate coord(DEG2RAD(-90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), -1.0, 1e-14);
    }
    
    // 北半球中纬度
    {
        GeoCoordinate coord(DEG2RAD(45.0), DEG2RAD(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_GT(zenith.z(), 0.0);  // z分量应为正
    }
    
    // 南半球中纬度
    {
        GeoCoordinate coord(DEG2RAD(-45.0), DEG2RAD(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_LT(zenith.z(), 0.0);  // z分量应为负
    }
}

// 测试天底方向向量
TEST(GeoCoordinateTest, GetNadir)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d zenith = coord.getZenith();
    Vector3d nadir = coord.getNadir();
    
    EXPECT_NEAR(nadir.x(), -zenith.x(), 1e-14);
    EXPECT_NEAR(nadir.y(), -zenith.y(), 1e-14);
    EXPECT_NEAR(nadir.z(), -zenith.z(), 1e-14);
}

// 测试东方向向量
TEST(GeoCoordinateTest, GetEast)
{
    // 经度0度
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.x(), 0.0, 1e-14);
        EXPECT_NEAR(east.y(), 1.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
    }
    
    // 经度90度
    {
        GeoCoordinate coord(0.0, DEG2RAD(90.0), 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.x(), -1.0, 1e-14);
        EXPECT_NEAR(east.y(), 0.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
    }
    
    // 南半球东方向
    {
        GeoCoordinate coord(DEG2RAD(-30.0), DEG2RAD(45.0), 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);  // 东方向始终水平
    }
}

// 测试西方向向量
TEST(GeoCoordinateTest, GetWest)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d east = coord.getEast();
    Vector3d west = coord.getWest();
    
    EXPECT_NEAR(west.x(), -east.x(), 1e-14);
    EXPECT_NEAR(west.y(), -east.y(), 1e-14);
    EXPECT_NEAR(west.z(), -east.z(), 1e-14);
}

// 测试北方向向量
TEST(GeoCoordinateTest, GetNorth)
{
    // 赤道上
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.x(), 0.0, 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), 1.0, 1e-14);
    }
    
    // 北半球中纬度
    {
        GeoCoordinate coord(DEG2RAD(45.0), 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_GT(north.z(), 0.0);  // z分量应为正
    }
    
    // 南半球
    {
        GeoCoordinate coord(DEG2RAD(-45.0), 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_GT(north.z(), 0.0);  // 北方向始终指向北极
    }
}

// 测试南方向向量
TEST(GeoCoordinateTest, GetSouth)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d north = coord.getNorth();
    Vector3d south = coord.getSouth();
    
    EXPECT_NEAR(south.x(), -north.x(), 1e-14);
    EXPECT_NEAR(south.y(), -north.y(), 1e-14);
    EXPECT_NEAR(south.z(), -north.z(), 1e-14);
}

// 测试方向向量的正交性
TEST(GeoCoordinateTest, Orthogonality)
{
    // 北半球
    {
        GeoCoordinate coord(DEG2RAD(30.0), DEG2RAD(45.0), 0.0);
        
        Vector3d zenith = coord.getZenith();
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        
        // 检查单位向量
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        
        // 检查正交性
        EXPECT_NEAR(zenith.dot(east), 0.0, 1e-14);
        EXPECT_NEAR(zenith.dot(north), 0.0, 1e-14);
        EXPECT_NEAR(east.dot(north), 0.0, 1e-14);
        
        // 检查右手系：east × north = zenith
        Vector3d cross = east.cross(north);
        EXPECT_NEAR(cross.x(), zenith.x(), 1e-14);
        EXPECT_NEAR(cross.y(), zenith.y(), 1e-14);
        EXPECT_NEAR(cross.z(), zenith.z(), 1e-14);
    }
    
    // 南半球
    {
        GeoCoordinate coord(DEG2RAD(-30.0), DEG2RAD(45.0), 0.0);
        
        Vector3d zenith = coord.getZenith();
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        
        EXPECT_NEAR(zenith.dot(east), 0.0, 1e-14);
        EXPECT_NEAR(zenith.dot(north), 0.0, 1e-14);
        EXPECT_NEAR(east.dot(north), 0.0, 1e-14);
    }
}

// 测试地心纬度到大地纬度转换
TEST(GeoCoordinateTest, SurfaceCentricToDeticLat)
{
    // 地球扁率因子 (WGS84)
    double flatFact = 1.0 / 298.257223563;
    
    // 赤道上，纬度不变
    double centricLat1 = 0.0;
    double deticLat1 = aSurfaceCentricToDeticLat(centricLat1, flatFact);
    EXPECT_NEAR(deticLat1, 0.0, 1e-14);
    
    // 中纬度（北半球），大地纬度应大于地心纬度
    double centricLat2 = DEG2RAD(45.0);
    double deticLat2 = aSurfaceCentricToDeticLat(centricLat2, flatFact);
    EXPECT_GT(deticLat2, centricLat2);
    EXPECT_LT(deticLat2, DEG2RAD(90.0));  // 确保在有效范围内
    
    // 中纬度（南半球）
    double centricLat3 = DEG2RAD(-45.0);
    double deticLat3 = aSurfaceCentricToDeticLat(centricLat3, flatFact);
    EXPECT_LT(deticLat3, centricLat3);  // 南半球：大地纬度 < 地心纬度
    EXPECT_GT(deticLat3, DEG2RAD(-90.0));  // 确保在有效范围内
    
    // 避开极点附近的数值问题，测试高纬度但不极点
    double centricLat4 = DEG2RAD(85.0);
    double deticLat4 = aSurfaceCentricToDeticLat(centricLat4, flatFact);
    EXPECT_GT(deticLat4, centricLat4);
    EXPECT_LT(deticLat4, DEG2RAD(90.0));
}

// 测试大地纬度到地心纬度转换
TEST(GeoCoordinateTest, SurfaceDeticToCentricLat)
{
    double flatFact = 1.0 / 298.257223563;
    
    // 赤道上
    double deticLat1 = 0.0;
    double centricLat1 = aSurfaceDeticToCentricLat(deticLat1, flatFact);
    EXPECT_NEAR(centricLat1, 0.0, 1e-14);
    
    // 中纬度（北半球），地心纬度应小于大地纬度
    double deticLat2 = DEG2RAD(45.0);
    double centricLat2 = aSurfaceDeticToCentricLat(deticLat2, flatFact);
    EXPECT_LT(centricLat2, deticLat2);
    
    // 中纬度（南半球）
    double deticLat3 = DEG2RAD(-45.0);
    double centricLat3 = aSurfaceDeticToCentricLat(deticLat3, flatFact);
    EXPECT_GT(centricLat3, deticLat3);  // 南半球：地心纬度 > 大地纬度
    
    // 高纬度但非极点
    double deticLat4 = DEG2RAD(85.0);
    double centricLat4 = aSurfaceDeticToCentricLat(deticLat4, flatFact);
    EXPECT_LT(centricLat4, deticLat4);
    EXPECT_GT(centricLat4, DEG2RAD(0.0));  // 仍为正值
}

// 测试双向转换的可逆性
TEST(GeoCoordinateTest, CentricDeticReversibility)
{
    double flatFact = 1.0 / 298.257223563;
    
    // 测试多个纬度（北半球和南半球）
    double testLats[] = {0.0, 15.0, 30.0, 45.0, 60.0, 75.0, -15.0, -30.0, -45.0, -60.0, -75.0};
    for (double latDeg : testLats)
    {
        double deticLat = DEG2RAD(latDeg);
        double centricLat = aSurfaceDeticToCentricLat(deticLat, flatFact);
        double deticLat2 = aSurfaceCentricToDeticLat(centricLat, flatFact);
        EXPECT_NEAR(deticLat2, deticLat, 1e-14) << "Failed at latitude " << latDeg << " degrees";
    }
}

GTEST_MAIN()
