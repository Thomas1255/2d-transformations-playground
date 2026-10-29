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

