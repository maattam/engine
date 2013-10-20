#include "mathelp.h"

#include <qmath.h>

const QVector3D UNIT_X(1, 0, 0);
const QVector3D UNIT_Y(0, 1, 0);
const QVector3D UNIT_Z(0, 0, 1);

// Precondition: start and end can't point in opposite directions.
QQuaternion rotationBetweenVectors(const QVector3D& start, const QVector3D& end)
{
    float theta = QVector3D::dotProduct(start, end);

    QVector3D axis = QVector3D::crossProduct(start, end);
    float halfangle = qAcos(theta) / 2;

    return QQuaternion(qCos(halfangle), axis * qSin(halfangle));
}

QQuaternion orientationFromAxes(const QVector3D& x, const QVector3D& y, const QVector3D& z)
{
    QMatrix3x3 rot;
    rot(0, 0) = x.x();
    rot(1, 0) = x.y();
    rot(2, 0) = x.z();

    rot(0, 1) = y.x();
    rot(1, 1) = y.y();
    rot(2, 1) = y.z();

    rot(0, 2) = z.x();
    rot(1, 2) = z.y();
    rot(2, 2) = z.z();

    return orientationFromRotationMatrix(rot);
}

QQuaternion orientationFromRotationMatrix(const QMatrix3x3& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".
    // Source: Ogre3D

    QQuaternion quat;

    float trace = rot(0, 0) + rot(1, 1) + rot(2, 2);
    float root = 0.0f;
    // x y z
    float quatv[3] = { 0.0f, 0.0f, 0.0f };

    if(trace > 0.0f)
    {
        // |w| > 1/2
        root = qSqrt(trace + 1.0f); // 2w
        quat.setScalar(0.5f * root);
        root = 0.5f / root; // 1 / (4w)

        quatv[0] = (rot(2, 1) - rot(1, 2)) * root;
        quatv[1] = (rot(0, 2) - rot(2, 0)) * root;
        quatv[2] = (rot(1, 0) - rot(0, 1)) * root;
    }

    else
    {
        // |w| <= 1/2
        const size_t next[3] = { 1, 2, 0 };
        size_t i = 0;

        if(rot(1, 1) > rot(0, 0))
            i = 1;

        if(rot(2, 2) > rot(i, i))
            i = 2;

        size_t j = next[i];
        size_t k = next[j];

        root = qSqrt(rot(i, i) - rot(j, j) - rot(k, k) + 1.0f);
        quatv[i] = 0.5f * root;
        root = 0.5f / root;

        quat.setScalar((rot(k, j) - rot(j, k)) * root);
        quatv[j] = (rot(j, i) + rot(i, j)) * root;
        quatv[k] = (rot(k, i) + rot(i, k)) * root;
    }

    quat.setVector(quatv[0], quatv[1], quatv[2]);
    return quat;
}