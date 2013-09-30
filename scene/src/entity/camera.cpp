#include "camera.h"

#include <QMatrix4x4>
#include <qmath.h>

using namespace Engine;
using namespace Engine::Entity;

Camera::Camera(const QVector3D& position, float horizontalAngle, float verticalAngle, float fov, float aspect, float farPlane)
    : position_(position), horizontal_(horizontalAngle), vertical_(verticalAngle), fov_(fov), aspect_(aspect), farPlane_(farPlane)
{
}

void Camera::move(const QVector3D& offset)
{
    position_ += offset;
}

void Camera::moveTo(const QVector3D& position)
{
    position_ = position;
}

void Camera::tilt(float horizontal, float vertical)
{
    horizontal_ += horizontal;
    vertical_ += vertical;
}

void Camera::setTilt(float horizontal, float vertical)
{
    horizontal_ = horizontal;
    vertical_ = vertical;
}

void Camera::setAspectRatio(float ratio)
{
    aspect_ = ratio;
}

QVector3D Camera::direction() const
{
    // Spherical coordinates to cartesian
    return QVector3D(
        cosf(vertical_) * sinf(horizontal_),
        sinf(vertical_),
        cosf(vertical_) * cosf(horizontal_));
}

QVector3D Camera::right() const
{
    return QVector3D(sin(horizontal_ - M_PI / 2.0f),
        0, cos(horizontal_ - M_PI / 2.0f));
}

QMatrix4x4 Camera::perspective() const
{
    QMatrix4x4 mat;
    mat.perspective(fov_, aspect_, 0.1f, farPlane_);

    return mat;
}

QMatrix4x4 Camera::lookAt() const
{
    QVector3D dir = direction();

    QMatrix4x4 mat;
    mat.lookAt(position_, position_ + dir, QVector3D::crossProduct(right(), dir));
    return mat;
}

const QVector3D& Camera::position() const
{
    return position_;
}

float Camera::horizontalAngle() const
{
    return horizontal_;
}

float Camera::verticalAngle() const
{
    return vertical_;
}