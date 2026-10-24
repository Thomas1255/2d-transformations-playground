#include "Transform2D.h"

#include <array>
#include <cmath>
#include <vector>

namespace math2d
{

    namespace
    {
        constexpr float kPi = 3.14159265358979323846f;

        /*
            Converts an angle from degrees to radians.

            Input:
                degrees : angle in degrees

            Output:
                angle in radians
        */
        float degToRad(float degrees)
        {
            return degrees * kPi / 180.0f;
        }
    }

    /*
        Returns the 3x3 identity matrix.

        Input:
            none

        Output:
            I =
            [1 0 0
             0 1 0
             0 0 1]
    */
    Mat3 makeIdentity()
    {
        Mat3 I{};

        I.m[0][0] = 1; // [ 1 0 0
        I.m[0][1] = 0;
        I.m[0][2] = 0;

        I.m[1][0] = 0; //  0 1 0
        I.m[1][1] = 1;
        I.m[1][2] = 0;

        I.m[2][0] = 0; //  0 0 1 ]
        I.m[2][1] = 0;
        I.m[2][2] = 1;
        return I;
    }

    /*
        Builds a 2D homogeneous translation matrix.

        Input:
            tx : translation amount in x
            ty : translation amount in y

        Output:
            T(tx, ty) =
            [1 0 tx
             0 1 ty
             0 0  1]
    */
    Mat3 makeTranslation(float tx, float ty)
    {
        // Start from the identity matrix and place tx, ty
        // in the translation column.
        Mat3 translation = makeIdentity();
        translation.m[0][2] = tx;
        translation.m[1][2] = ty;
        return translation;
    }

    /*
        Builds a 2D homogeneous scaling matrix.

        Input:
            sx : scale factor in x
            sy : scale factor in y

        Output:
            S(sx, sy) =
            [sx  0  0
              0 sy  0
              0  0  1]
    */
    Mat3 makeScale(float sx, float sy)
    {
        // Put sx and sy on the diagonal in the upper-left 2x2 block.
        Mat3 scale = makeIdentity();
        scale.m[0][0] = sx;
        scale.m[1][1] = sy;
        return scale;
    }

    /*
        Builds a 2D homogeneous rotation matrix using an angle in degrees.

        Input:
            degrees : counterclockwise rotation angle in degrees

        Output:
            R(theta) =
            [ cos(theta) -sin(theta) 0
              sin(theta)  cos(theta) 0
                   0           0     1 ]
    */
    Mat3 makeRotationDeg(float degrees)
    {
        // 1. Convert degrees to radians.
        // 2. Compute cos(theta) and sin(theta).
        // 3. Fill the standard 2D rotation matrix.
        float rad = degToRad(degrees);

        float cos = std::cos(rad);
        float sin = std::sin(rad);

        Mat3 rotation = makeIdentity();
        rotation.m[0][0] = cos;
        rotation.m[0][1] = -sin;
        rotation.m[1][0] = sin;
        rotation.m[1][1] = cos;
        return rotation;
    }

    /*
        Builds a 2D homogeneous x-shear matrix.

        Input:
            kx : x-shear factor

        Output:
            Shx(kx) =
            [1 kx 0
             0  1 0
             0  0 1]
    */
    Mat3 makeShearX(float kx)
    {
        // put kx in the (0,1) entry of the matrix.
        Mat3 shearX = makeIdentity();
        shearX.m[0][1] = kx;

        return shearX;
    }

    /*
        Builds a 2D homogeneous y-shear matrix.

        Input:
            ky : y-shear factor

        Output:
            Shy(ky) =
            [1  0 0
             ky 1 0
             0  0 1]
    */
    Mat3 makeShearY(float ky)
    {
        // put ky in the (1,0) entry of the matrix
        Mat3 shearY = makeIdentity();
        shearY.m[1][0] = ky;
        return shearY;
    }

    /*
        Computes matrix-matrix multiplication C = A * B.

        Input:
            A : left  3x3 matrix
            B : right 3x3 matrix

        Output:
            C = A * B

        Notes:
            Use standard matrix multiplication:
            C[i][j] = sum_k A[i][k] * B[k][j]
    */
    Mat3 multiply(const Mat3 &A, const Mat3 &B)
    {
        Mat3 product;

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                product.m[i][j] = A.m[i][0] * B.m[0][j] + A.m[i][1] * B.m[1][j] + A.m[i][2] * B.m[2][j];
            }
        }

        return product;
    }

    /*
        Applies a homogeneous transformation matrix to a homogeneous vector.

        Input:
            M : 3x3 transformation matrix
            p : homogeneous input vector (x, y, w)

        Output:
            transformed vector M * p

        Important:
            - If w = 1, the input behaves like a point.
            - If w = 0, the input behaves like a direction vector.
            - Translation should affect points but not direction vectors.
    */
    Vec3 transform(const Mat3 &M, const Vec3 &p)
    {
        Vec3 transformation;

        transformation.x = M.m[0][0] * p.x + M.m[0][1] * p.y + M.m[0][2] * p.w;
        transformation.y = M.m[1][0] * p.x + M.m[1][1] * p.y + M.m[1][2] * p.w;
        transformation.w = M.m[2][0] * p.x + M.m[2][1] * p.y + M.m[2][2] * p.w;
        return transformation;
    }

    /*
        Builds a rotation matrix about a pivot point (cx, cy).

        Input:
            degrees : rotation angle in degrees
            cx, cy  : pivot coordinates

        Output:
            matrix for rotation about the pivot

        Required form:
            T(cx, cy) * R(degrees) * T(-cx, -cy)
    */
    Mat3 makeRotationAboutPivot(float degrees, float cx, float cy)
    {
        Mat3 T_1 = makeTranslation(cx, cy);
        Mat3 R = makeRotationDeg(degrees);
        Mat3 T_2 = makeTranslation(-cx, -cy);

        return multiply(multiply(T_1, R), T_2); // T_1 * R * T_2
    }

    /*
        Builds a scaling matrix about a pivot point (cx, cy).

        Input:
            sx, sy  : scale factors
            cx, cy  : pivot coordinates

        Output:
            matrix for scaling about the pivot

        Required form:
            T(cx, cy) * S(sx, sy) * T(-cx, -cy)
    */
    Mat3 makeScaleAboutPivot(float sx, float sy, float cx, float cy)
    {
        Mat3 T_1 = makeTranslation(cx, cy);
        Mat3 S = makeScale(sx, sy);
        Mat3 T_2 = makeTranslation(-cx, -cy);

        return multiply(multiply(T_1, S), T_2);
    }

    /*
        Builds the main transformation matrix used by the app.

        Input:
            tx, ty        : translation amounts
            rotationDeg   : rotation angle in degrees
            sx, sy        : scale factors
            shearX, shearY: shear factors

        Output:
            final composed matrix M

        Required order:
            M = T(tx, ty) * R(rotationDeg) * ShY(shearY) * ShX(shearX) * S(sx, sy)

        Notes:
            With column vectors and left multiplication, the matrix closest
            to the input acts first.
    */
    Mat3 buildStandardTransform(float tx, float ty,
                                float rotationDeg,
                                float sx, float sy,
                                float shearX, float shearY,
                                bool pivotRotation, bool pivotScale,
                                float pivotX, float pivotY)
    {
        // M = T * R * ShY * ShX * S
        Mat3 T = makeTranslation(tx, ty);

        // R and S are chosen conditionally:
        // pivot off -> plain origin-based matrix
        // pivot on  -> T(p), act, T back. Pivot point stays fixed.
        Mat3 R = pivotRotation
            ? makeRotationAboutPivot(rotationDeg, pivotX, pivotY)
            : makeRotationDeg(rotationDeg);

        Mat3 ShY = makeShearY(shearY);
        Mat3 ShX = makeShearX(shearX);

        Mat3 S = pivotScale
            ? makeScaleAboutPivot(sx, sy, pivotX, pivotY)
            : makeScale(sx, sy);

        Mat3 stdTransformation;
        stdTransformation = multiply(T, R);
        stdTransformation = multiply(stdTransformation, ShY);
        stdTransformation = multiply(stdTransformation, ShX);
        stdTransformation = multiply(stdTransformation, S);
        return stdTransformation;
    }

    /*
        Builds only the linear part of the transformation.

        Input:
            rotationDeg   : rotation angle in degrees
            sx, sy        : scale factors
            shearX, shearY: shear factors

        Output:
            linear transformation matrix without translation

        Required order:
            M = R(rotationDeg) * ShY(shearY) * ShX(shearX) * S(sx, sy)

        Notes:
            The app uses this for basis-vector visualization.
    */
    Mat3 buildLinearOnlyTransform(float rotationDeg,
                                  float sx, float sy,
                                  float shearX, float shearY)
    {
        // M = R * ShY * ShX * S
        Mat3 R = makeRotationDeg(rotationDeg);
        Mat3 ShY = makeShearY(shearY);
        Mat3 ShX = makeShearX(shearX);
        Mat3 S = makeScale(sx, sy);

        Mat3 linearOnlyTransformation;
        linearOnlyTransformation = multiply(R, ShY);
        linearOnlyTransformation = multiply(linearOnlyTransformation, ShX);
        linearOnlyTransformation = multiply(linearOnlyTransformation, S);
        return linearOnlyTransformation;
    }

    /*
        Creates a homogeneous point.

        Input:
            x, y : 2D point coordinates

        Output:
            (x, y, 1)
    */
    Vec3 makePoint(float x, float y)
    {
        return {x, y, 1.0f};
    }

    /*
        Creates a homogeneous direction vector.

        Input:
            x, y : 2D vector components

        Output:
            (x, y, 0)
    */
    Vec3 makeVector(float x, float y)
    {
        return {x, y, 0.0f};
    }

    /*
        Returns the predefined triangle shape in world coordinates.

        Input:
            none

        Output:
            vector of 2D vertices
    */
    std::vector<Vec2> makeTriangle()
    {
        return {
            {-2.0f, -1.4f},
            {2.0f, -1.4f},
            {0.0f, 2.2f}};
    }

    /*
        Returns the predefined square shape in world coordinates.

        Input:
            none

        Output:
            vector of 2D vertices
    */
    std::vector<Vec2> makeSquare()
    {
        return {
            {-2.0f, -2.0f},
            {2.0f, -2.0f},
            {2.0f, 2.0f},
            {-2.0f, 2.0f}};
    }

    /*
        Returns the predefined house shape in world coordinates.

        Input:
            none

        Output:
            vector of 2D vertices
    */
    std::vector<Vec2> makeHouse()
    {
        return {
            {-2.0f, -2.0f},
            {2.0f, -2.0f},
            {2.0f, 0.4f},
            {0.0f, 2.4f},
            {-2.0f, 0.4f}};
    }

    /*
        Returns the predefined car shape in world coordinates.

        Input:
            none

        Output:
            vector of 2D vertices
    */
    std::vector<Vec2> makeCar()
    {
        return {
            {-3.0f, -1.0f},
            {-2.2f, 0.4f},
            {-0.8f, 0.4f},
            {-0.2f, 1.2f},
            {1.4f, 1.2f},
            {2.2f, 0.4f},
            {3.2f, 0.4f},
            {3.2f, -1.0f},
            {-3.0f, -1.0f}};
    }

    /*
        Applies a transformation matrix to every vertex of a shape.

        Input:
            M   : 3x3 transformation matrix
            pts : list of 2D vertices

        Output:
            transformed list of 2D vertices

        Notes:
            Each 2D vertex is treated as a point, so use w = 1.
    */
    std::vector<Vec2> transformShape(const Mat3 &M, const std::vector<Vec2> &pts)
    {
        std::vector<Vec2> out;
        out.reserve(pts.size());

        for (const auto &p : pts)
        {
            const Vec3 hp = makePoint(p.x, p.y);
            const Vec3 tp = transform(M, hp);
            out.push_back({tp.x, tp.y});
        }

        return out;
    }

    /*
        Computes the transformed x-basis direction for visualization.

        Input:
            M : linear transformation matrix

        Output:
            two points:
            - the transformed origin
            - the transformed x-basis endpoint

        Notes:
            The app uses these two points to draw the transformed x-axis arrow.
    */
    std::array<Vec2, 2> transformedBasisX(const Mat3 &M)
    {
        const Vec3 origin = transform(M, makeVector(0.0f, 0.0f));
        const Vec3 ex = transform(M, makeVector(3.0f, 0.0f));
        return {{{origin.x, origin.y}, {ex.x, ex.y}}};
    }

    /*
        Computes the transformed y-basis direction for visualization.

        Input:
            M : linear transformation matrix

        Output:
            two points:
            - the transformed origin
            - the transformed y-basis endpoint

        Notes:
            The app uses these two points to draw the transformed y-axis arrow.
    */
    std::array<Vec2, 2> transformedBasisY(const Mat3 &M)
    {
        const Vec3 origin = transform(M, makeVector(0.0f, 0.0f));
        const Vec3 ey = transform(M, makeVector(0.0f, 3.0f));
        return {{{origin.x, origin.y}, {ey.x, ey.y}}};
    }

}