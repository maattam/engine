//
//  Author   : Matti Määttä
//  Summary  : Misc math operations
//

#ifndef MATHELP_H
#define MATHELP_H

#include <QMatrix3x3>
#include <QMatrix4x4>
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

extern QVector3D linearColor(const QVector3D& color, float gamma = 2.2f);

// Matrix4x4 helpers

extern QVector3D extractScale(const QMatrix4x4& mat);

extern QQuaternion extractOrientation(const QMatrix4x4& mat);

extern QVector3D extractTranslation(const QMatrix4x4& mat);

#endif // MATHELP_H