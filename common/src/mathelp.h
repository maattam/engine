#ifndef MATHELP_H
#define MATHELP_H

#include <QMatrix3x3>
#include <QVector3D>
#include <QQuaternion>

// Unit vectors
extern const QVector3D UNIT_X;
extern const QVector3D UNIT_Y;
extern const QVector3D UNIT_Z;

// Quaternion helpers

// Preconditions: start and end can't point in opposite directions.
//                start and end has to be normalized.
QQuaternion rotationBetweenVectors(const QVector3D& start, const QVector3D& end);

// Precondition: axes form an orthonormal basis
QQuaternion orientationFromAxes(const QVector3D& x, const QVector3D& y, const QVector3D& z);

// Precondition: matrix is orthogonal
QQuaternion orientationFromRotationMatrix(const QMatrix3x3& rot);

QVector3D linearColor(const QVector3D& color, float gamma = 2.2f);

#endif // MATHELP_H