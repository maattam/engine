#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

namespace Engine { namespace Entity {

class Camera
{
public:
    Camera(const QVector3D& position,
        float horizontalAngle = 0.0f, float verticalAngle = 0.0f);

    void move(const QVector3D& offset);
    void moveTo(const QVector3D& position);

    void tilt(float horizontal, float vertical);
    void setTilt(float horizontal, float vertical);

    float horizontalAngle() const;
    float verticalAngle() const;

    void setAspectRatio(float ratio);

    QMatrix4x4 perspective() const;
    QMatrix4x4 ortho(float width, float height) const;
    QMatrix4x4 lookAt() const;
    QMatrix4x4 lookAt(const QVector3D& direction) const;

    QVector3D direction() const;
    QVector3D right() const;
    const QVector3D& position() const;

    float farPlane() const;
    float nearPlane() const;

    void setFarPlane(float farPlane);
    void setNearPlane(float nearPlane);

    float fov() const;
    void setFov(float value);

private:
    QVector3D position_;

    float horizontal_;
    float vertical_;
    float fov_;
    float aspect_;
    float far_;
    float near_;
};

}}

#endif // CAMERA_H