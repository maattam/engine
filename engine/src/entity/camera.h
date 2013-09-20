#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

namespace Engine { namespace Entity {

    class Camera
    {
    public:
        Camera(const QVector3D& position, float horizontalAngle, float verticalAngle, float fov, float aspect);

        void move(const QVector3D& offset);
        void moveTo(const QVector3D& position);

        void tilt(float horizontal, float vertical);
        void setTilt(float horizontal, float vertical);

        void setAspectRatio(float ratio);

        QMatrix4x4 perspective() const;
        QMatrix4x4 lookAt() const;

        QVector3D direction() const;
        QVector3D right() const;
        const QVector3D& position() const;

    private:
        QVector3D position_;

        float horizontal_;
        float vertical_;
        float fov_;
        float aspect_;
    };

}}

#endif // CAMERA_H