#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QRect>

#include "mathelp.h"

namespace Engine { namespace Entity {

class Camera
{
public:
    enum CameraType { PERSPECTIVE, ORTHOGRAPHIC };

    // The camera points towards positive z by default. The up is (0, 1, 0).
    Camera(CameraType type, const QVector3D& direction = UNIT_Z);

    // Constructs a camera with perspective projection.
    Camera(float aspectRatio, float fov, const QVector3D& direction = UNIT_Z);

    // Constructs a camera with orthogonal projection.
    Camera(const QRectF& window, const QVector3D& direction = UNIT_Z);

    CameraType type() const;

    void setPosition(const QVector3D& position);
    const QVector3D& position() const;

    // Adds offset to the camera's position
    void move(const QVector3D& offset);

    // Moves relative to the internal orientation.
    void moveRelative(const QVector3D& offset);

    void rotate(const QQuaternion& quaternion);
    void rotate(float angle, const QVector3D& axis);

    // Rotates camera alongs local X axis
    void pitch(float angle);

    // Rotates camera along local Z axis
    void roll(float angle);

    // Rotates camera along the local Y axis, or if
    // setFixedYaw has been set, rotates along that axis.
    void yaw(float angle);

    void setOrientation(const QQuaternion& quaternion);
    const QQuaternion& orientation() const;

    // Sets the camera's direction to point towards the given direction
    // The underlying rotation is reset and is not cumulative.
    void setDirection(const QVector3D& direction);

    // Returns an unit vector pointing towards the camera's target
    QVector3D direction() const;

    QVector3D up() const;
    QVector3D right() const;

    // Helper function that sets the camera's direction based on the current position.
    void lookAt(const QVector3D& target);

    // Sets the camera's bounds for orthographic projection.
    // Has no effect if the camera's type is PERSPECTIVE.
    void setWindow(const QRectF& window);

    // Calculates the camera's view matrix using orientation * translation
    QMatrix4x4 view() const;

    // Calculates the camera's projection matrix. Depends on the camera's type.
    QMatrix4x4 projection() const;

    // Returns cached projection * view matrix.
    const QMatrix4x4& worldView() const;

    void setAspectRatio(float ratio);
    void setFarPlane(float farPlane);
    void setNearPlane(float nearPlane);
    void setFov(float value);

    void setFixedYaw(bool value, const QVector3D& yaw = UNIT_Y);

    // Calculates and caches the camera's world view matrix
    void update();

private:
    CameraType type_;
    QVector3D position_;
    QQuaternion orientation_;
    QRectF bounds_;

    QVector3D fixedYawAxis_;
    bool fixedYaw_;

    QMatrix4x4 worldView_;

    float fov_;
    float aspect_;
    float far_;
    float near_;
};

}}

#endif // CAMERA_H