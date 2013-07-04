#include "light.h"

using namespace Engine;

Light::Light(const QVector3D& color, const QVector3D& position, float intensity)
    : color_(color), position_(position)
{
    setIntensity(intensity);
}

void Light::setPosition(const QVector3D& position)
{
    position_ = position;
}

void Light::setColor(const QVector3D& color)
{
    color_ = color;
}

void Light::setIntensity(float intensity)
{
    if(intensity < 0.0f)
    {
        intensity_ = 0.0f;
    }

    else
    {
        intensity_ = intensity;
    }
}

const QVector3D& Light::position() const
{
    return position_;
}

const QVector3D& Light::color() const
{
    return color_;
}

float Light::intensity() const
{
    return intensity_;
}