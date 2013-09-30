// Defines light data structures

#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

namespace Engine { namespace Entity {

struct Light
{
    QVector3D color;

    float ambientIntensity;
    float diffuseIntensity;

    Light()
    {
        color = QVector3D(1.0f, 1.0f, 1.0f);
        ambientIntensity = 0.0f;
        diffuseIntensity = 1.0f;
    }
};

struct DirectionalLight : public Light
{
    QVector3D direction;

    DirectionalLight() : Light()
    {
        direction = QVector3D(0.0f, 0.0f, 0.0f);
    }
};

struct PointLight : public Light
{
    QVector3D position;

    struct Attenuation
    {
        float constant;
        float linear;
        float exp;
    } attenuation;

    PointLight() : Light()
    {
        position = QVector3D(0.0f, 0.0f, 0.0f);
        attenuation.constant = 0.0f;
        attenuation.linear = 0.0f;
        attenuation.exp = 1.0f;
    }
};

struct SpotLight : public PointLight
{
    float cutoff;
    QVector3D direction;

    SpotLight() : PointLight()
    {
        cutoff = 0.0f;
        direction = QVector3D(0.0f, 0.0f, 0.0f);
    }
};

}}

#endif //LIGHT_H