#include "camera.h"

#include "graph/scenenode.h"

#include <QMatrix4x4>
#include <qmath.h>

using namespace Engine;
using namespace Engine::Graph;

Camera::Camera(CameraType type, const QVector3D& dir)
    : SceneLeaf(), type_(type)
{
    far_ = 500.0f;
    near_ = 0.1f;
    fov_ = 45.0f;
    aspect_ = 1.0f;

    setFixedYaw(true);
    setDirection(dir);
    update();
}

Camera::Camera(float aspectRatio, float fov, const QVector3D& dir)
    : SceneLeaf(), type_(PERSPECTIVE), aspect_(aspectRatio), fov_(fov)
{
    far_ = 500.0f;
    near_ = 0.1f;

    setFixedYaw(true);
    setDirection(dir);
    update();
}

Camera::Camera(const QRectF& window, const QVector3D& dir)
    : SceneLeaf(), type_(ORTHOGRAPHIC), bounds_(window)
{
    far_ = 500.0f;
    near_ = 0.1f;

    setFixedYaw(true);
    setDirection(dir);
    update();
}

Camera::~Camera()
{
}

void Camera::setPosition(const QVector3D& position)
{
    position_ = position;
}

const QVector3D& Camera::position() const
{
    return position_;
}

void Camera::move(const QVector3D& offset)
{
    position_ += offset;
}

void Camera::moveRelative(const QVector3D& offset)
{
    QVector3D trans = orientation_.rotatedVector(offset);
    position_ += trans;
}

void Camera::rotate(const QQuaternion& quaternion)
{
    orientation_ = quaternion.normalized() * orientation_;
}

void Camera::rotate(float angle, const QVector3D& axis)
{
    rotate(QQuaternion::fromAxisAndAngle(axis, angle));
}

const QQuaternion& Camera::orientation() const
{
    return orientation_;
}

void Camera::setOrientation(const QQuaternion& quaternion)
{
    orientation_ = quaternion.normalized();
}

void Camera::setDirection(const QVector3D& direction)
{
    // Default look in OpenGL is towards -Z
    const QVector3D start = -UNIT_Z;
    const QVector3D dest = direction.normalized();

    // 180 degree turn; infite possibilites, choose one that works.
    if(QVector3D::dotProduct(start, dest) < -1.0f + 0.001f)
    {
        orientation_ = QQuaternion(0.0f, 0.0f, 1.0f, 0.0f);
    }

    else
    {
        orientation_ = rotationBetweenVectors(start, dest);
    }

    // TODO: Transform to parent space when the orientation is derived from parent node

    orientation_.normalize();
}

void Camera::lookAt(const QVector3D& target)
{
    setDirection(target - position_);
}

const QMatrix4x4& Camera::worldView() const
{
    return worldView_;
}

void Camera::update()
{
    worldView_ = projection() * view();
}

QMatrix4x4 Camera::view() const
{
    // View matrix is:
    //
    //  [ Lx  Uy  Dz  Tx  ]
    //  [ Lx  Uy  Dz  Ty  ]
    //  [ Lx  Uy  Dz  Tz  ]
    //  [ 0   0   0   1   ]
    //
    // Where T = -(Transposed(Rot) * Pos)

    QMatrix4x4 view;
    view.lookAt(position(), position() + direction(), up());
    return view;
}

QMatrix4x4 Camera::projection() const
{
    QMatrix4x4 proj;

    if(type_ == PERSPECTIVE)
    {
        proj.perspective(fov_, aspect_, near_, far_);
    }

    else
    {
        proj.ortho(bounds_.left(), bounds_.right(), bounds_.bottom(),
            bounds_.top(), near_, far_);
    }

    return proj;
}

void Camera::setAspectRatio(float ratio)
{
    aspect_ = ratio;
}

void Camera::setFov(float fov)
{
    fov_ = fov;
}

void Camera::setFarPlane(float far)
{
    far_ = far;
}

void Camera::setNearPlane(float near)
{
    near_ = near;
}

float Camera::nearPlane() const
{
    return near_;
}

float Camera::farPlane() const
{
    return far_;
}

QVector3D Camera::up() const
{
    // Default up is +Y, so we rotate +Y to find the current up.
    return orientation_.rotatedVector(UNIT_Y);
}

QVector3D Camera::right() const
{
    // Default right is +X
    return orientation_.rotatedVector(UNIT_X);
}

QVector3D Camera::direction() const
{
    // Default direction points towards -Z
    return orientation_.rotatedVector(-UNIT_Z);
}

void Camera::setFixedYaw(bool value, const QVector3D& yaw)
{
    fixedYaw_ = value;
    fixedYawAxis_ = yaw.normalized();
}

void Camera::pitch(float angle)
{
    rotate(angle, orientation_.rotatedVector(UNIT_X));
}

void Camera::roll(float angle)
{
    rotate(angle, orientation_.rotatedVector(UNIT_Z));
}

void Camera::yaw(float angle)
{
    QVector3D axis;

    if(fixedYaw_)
    {
        axis = fixedYawAxis_;
    }

    else
    {
        // If axis is not fixed, we rotate by the local Y
        axis = orientation_.rotatedVector(UNIT_Y);
    }

    rotate(angle, axis);
}