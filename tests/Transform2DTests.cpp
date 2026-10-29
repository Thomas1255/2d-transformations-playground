// Unit tests for the math2d library (src/math/Transform2D.{h,cpp}).
//
// Framework: Google Test (compiled lib, declared in vcpkg.json manifest;
// linked as GTest::gtest_main which supplies main() for us).
// Run with:  build.cmd test   (or `ctest --preset release` inside build/)
//
// Test Description:
//   Each test proves ONE documented guarantee from the math library's
//   comment headers, e.g. "translation affects points (w=1) but not
//   direction vectors (w=0)" or the composition order M = T * R * ShY * ShX * S.
//   Float math is never compared with == and every comparison goes through
//   EXPECT_NEAR with an epsilon.

#include "math/Transform2D.h"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
    // Float ops (float trig) accumulate tiny errors, so
    // use small epsilon to compare instead of == 
    constexpr float kEps = 1e-4f;

    void expectSameMatrix(const math2d::Mat3 &A, const math2d::Mat3 &B)
    {
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                EXPECT_NEAR(A.m[r][c], B.m[r][c], kEps) << "at (row " << r << ", col " << c << ")";
            }
        }
    }

    void expectSameVector(const math2d::Vec3 &a, const math2d::Vec3 &b)
    {
        EXPECT_NEAR(a.x, b.x, kEps);
        EXPECT_NEAR(a.y, b.y, kEps);
        EXPECT_NEAR(a.w, b.w, kEps);
    }
}

// ---------------------------------------------------------------------------
// Constructors: exact matrix layouts
// ---------------------------------------------------------------------------

TEST(Transform2D, makeIdentityReturnsTheIdentityMatrix)
{
    const math2d::Mat3 I = math2d::makeIdentity();

    EXPECT_NEAR(I.m[0][0], 1.0f, kEps);
    EXPECT_NEAR(I.m[0][1], 0.0f, kEps);
    EXPECT_NEAR(I.m[0][2], 0.0f, kEps);

    EXPECT_NEAR(I.m[1][0], 0.0f, kEps);
    EXPECT_NEAR(I.m[1][1], 1.0f, kEps);
    EXPECT_NEAR(I.m[1][2], 0.0f, kEps);

    EXPECT_NEAR(I.m[2][0], 0.0f, kEps);
    EXPECT_NEAR(I.m[2][1], 0.0f, kEps);
    EXPECT_NEAR(I.m[2][2], 1.0f, kEps);
}

TEST(Transform2D, makeTranslationPutsTxTyInTheTranslationColumn)
{
    const math2d::Mat3 T = math2d::makeTranslation(4.0f, -2.0f);

    // The linear block stays identity; only column 2 carries the offset.
    EXPECT_NEAR(T.m[0][0], 1.0f, kEps);
    EXPECT_NEAR(T.m[1][1], 1.0f, kEps);
    EXPECT_NEAR(T.m[0][2], 4.0f, kEps);
    EXPECT_NEAR(T.m[1][2], -2.0f, kEps);
}

TEST(Transform2D, makeScalePutsSxSyOnTheDiagonal)
{
    const math2d::Mat3 S = math2d::makeScale(3.0f, 0.5f);

    EXPECT_NEAR(S.m[0][0], 3.0f, kEps);
    EXPECT_NEAR(S.m[1][1], 0.5f, kEps);
    EXPECT_NEAR(S.m[0][1], 0.0f, kEps); // must NOT be transposed
    EXPECT_NEAR(S.m[1][0], 0.0f, kEps);
}

// ---------------------------------------------------------------------------
// transform: the point / direction-vector distinction (homogeneous w)
// ---------------------------------------------------------------------------

TEST(Transform2D, transformMovesPointsButNotDirectionVectors)
{
    const math2d::Mat3 T = math2d::makeTranslation(10.0f, 20.0f);

    const math2d::Vec3 point  = math2d::transform(T, math2d::makePoint(1.0f, 1.0f));
    const math2d::Vec3 vector = math2d::transform(T, math2d::makeVector(1.0f, 1.0f));

    EXPECT_NEAR(point.x, 11.0f, kEps);
    EXPECT_NEAR(point.y, 21.0f, kEps);
    EXPECT_NEAR(point.w, 1.0f, kEps);

    // A direction has no position, so translation must change nothing about it.
    EXPECT_NEAR(vector.x, 1.0f, kEps);
    EXPECT_NEAR(vector.y, 1.0f, kEps);
    EXPECT_NEAR(vector.w, 0.0f, kEps);
}

TEST(Transform2D, transformScalingIsLinearAndHitsPointsAndVectorsAlike)
{
    const math2d::Mat3 S = math2d::makeScale(2.0f, -3.0f);

    const math2d::Vec3 point  = math2d::transform(S, math2d::makePoint(4.0f, 1.0f));
    const math2d::Vec3 vector = math2d::transform(S, math2d::makeVector(4.0f, 1.0f));

    EXPECT_NEAR(point.x, 8.0f, kEps);
    EXPECT_NEAR(point.y, -3.0f, kEps);
    EXPECT_NEAR(vector.x, point.x, kEps);
    EXPECT_NEAR(vector.y, point.y, kEps);
}

// ---------------------------------------------------------------------------
// Rotation (about the origin)
// ---------------------------------------------------------------------------

TEST(Transform2D, rotationOfZeroDegreesIsTheIdentity)
{
    const math2d::Mat3 R0 = math2d::makeRotationDeg(0.0f);
    const math2d::Mat3 I  = math2d::makeIdentity();

    expectSameMatrix(R0, I);
}

TEST(Transform2D, rotationOf90DegreesMapsTheXAxisOntoTheYAxis)
{
    const math2d::Mat3 R90 = math2d::makeRotationDeg(90.0f);

    const math2d::Vec3 xAxis = math2d::transform(R90, math2d::makeVector(1.0f, 0.0f));
    const math2d::Vec3 yAxis = math2d::transform(R90, math2d::makeVector(0.0f, 1.0f));

    // Degenerate check: sin(90) = 1 is exact-ish, cos(90) rounds to 0.
    EXPECT_NEAR(xAxis.x, 0.0f, kEps);
    EXPECT_NEAR(xAxis.y, 1.0f, kEps);

    EXPECT_NEAR(yAxis.x, -1.0f, kEps);
    EXPECT_NEAR(yAxis.y, 0.0f, kEps);
}

TEST(Transform2D, rotationOf180DegreesMapsEveryPointToItsNegation)
{
    const math2d::Mat3 R180 = math2d::makeRotationDeg(180.0f);

    const math2d::Vec3 q = math2d::transform(R180, math2d::makePoint(2.0f, 5.0f));

    EXPECT_NEAR(q.x, -2.0f, kEps);
    EXPECT_NEAR(q.y, -5.0f, kEps);
}

