#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

namespace Engine {

class Light
{
public:
    Light(const QVector3D& color, const QVector3D& position, float intensity);

    void setPosition(const QVector3D& position);
    void setColor(const QVector3D& color);
    void setIntensity(float power);

    const QVector3D& position() const;
    const QVector3D& color() const;
    float intensity() const;

private:
    QVector3D color_;
    QVector3D position_;
    float intensity_;
};

}

#endif //LIGHT_H