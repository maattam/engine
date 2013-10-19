#include "light.h"

using namespace Engine::Entity;

Light::Light(LightType type) : Entity(), type_(type)
{
    color = QVector3D(1.0f, 1.0f, 1.0f);

    ambientIntensity = 0.0f;
    diffuseIntensity = 1.0f;

    attenuation.constant = 0.0f;
    attenuation.linear = 0.0f;
    attenuation.exp = 1.0f;
}

Light::LightType Light::type() const
{
    return type_;
}

void Light::updateRenderList(Engine::RenderList& list)
{
}