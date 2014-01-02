#include "basiclightning.h"

#include "entity/light.h"

#include <QDebug>
#include "mathelp.h"

#include <qmath.h>
#include <sstream>
#include <cassert>

using namespace Engine;
using namespace Engine::Technique;

BasicLightning::BasicLightning()
    : Technique()
{
}

BasicLightning::~BasicLightning()
{
}

bool BasicLightning::init()
{
    return true;
}

void BasicLightning::setMVP(const QMatrix4x4& mvp)
{
    setUniformValue("gMVP", mvp);
}

void BasicLightning::setWorldView(const QMatrix4x4& vp)
{
    setUniformValue("gWorld", vp);
}

void BasicLightning::setEyeWorldPos(const QVector3D& eyePos)
{
    setUniformValue("gEyeWorldPos", eyePos);
}

void BasicLightning::setHasTangents(bool tangents)
{
    setUniformValue("gHasTangents", tangents);
}

void BasicLightning::setMaterialAttributes(const Material::Attributes& attributes)
{
    setUniformValue("gMaterial.ambientColor", linearColor(attributes.ambientColor));
    setUniformValue("gMaterial.diffuseColor", linearColor(attributes.diffuseColor));
    setUniformValue("gMaterial.specularIntensity", attributes.specularIntensity);
    setUniformValue("gMaterial.shininess", attributes.shininess);
}

void BasicLightning::setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular, GLuint mask)
{
    setUniformValue("gDiffuseSampler", diffuse);
    setUniformValue("gNormalSampler", normal);
    setUniformValue("gSpecularSampler", specular);
    setUniformValue("gMaskSampler", mask);
}

void BasicLightning::setDirectionalLight(Entity::Light* light)
{
    float ambientFactor = 0.0f;
    if(light->diffuseIntensity() > 0.0f)
    {
        ambientFactor = light->ambientIntensity() / light->diffuseIntensity();
    }

    setUniformValue("gDirectionalLight.ambientIntensity", ambientFactor);
    setUniformValue("gDirectionalLight.base.color", linearColor(light->color()) * light->diffuseIntensity());
    setUniformValue("gDirectionalLight.direction", light->direction());
}

void BasicLightning::setSpotLightMVP(size_t index, const QMatrix4x4& mvp)
{
    assert(index < MAX_SPOT_LIGHTS);
    setUniformValue(formatUniformTableName("gLightMVP", index).c_str(), mvp);
}

void BasicLightning::setSpotLightShadowUnit(size_t index, GLuint shadow)
{
     assert(index < MAX_SPOT_LIGHTS);
     setUniformValue(formatUniformTableName("gSpotLightShadowMap", index).c_str(), shadow);
}

void BasicLightning::setDirectionalLightMVP(const QMatrix4x4& mvp)
{
    setUniformValue("gDirectionalLightMVP", mvp);
}

void BasicLightning::setDirectionalLightShadowUnit(GLuint shadow)
{
    setUniformValue("gDirectionalLightShadowMap", shadow);
}

void BasicLightning::setPointAndSpotLights(const QList<Entity::Light*>& lights)
{
    int numSpotLights = 0;
    int numPointLights = 0;

    for(const Entity::Light* light : lights)
    {
        if(light->type() == Entity::Light::LIGHT_POINT)
        {
            if(numPointLights >= MAX_POINT_LIGHTS)
            {
                qWarning() << __FUNCTION__ << "MAX_POINT_LIGHTS exceeded";
            }

            else
            {
                setPointLight(light, numPointLights++);
            }
        }

        else if(light->type() == Entity::Light::LIGHT_SPOT)
        {
            if(numSpotLights >= MAX_SPOT_LIGHTS)
            {
                qWarning() << __FUNCTION__ << "MAX_SPOT_LIGHTS exceeded";
            }

            else
            {
                setSpotLight(light, numSpotLights++);
            }
        }
    }

    setUniformValue("gNumPointLights", numPointLights);
    setUniformValue("gNumSpotLights", numSpotLights);
}

void BasicLightning::setSpotLight(const Entity::Light* light, int index)
{
    setUniformValue(formatUniformTableName("gSpotLights", index, "base.base.color").c_str(),             linearColor(light->color()) * light->diffuseIntensity());
    setUniformValue(formatUniformTableName("gSpotLights", index, "base.position").c_str(),               light->position());
    setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.constant").c_str(),   light->attenuation().constant);
    setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.exp").c_str(),        light->attenuation().quadratic);
    setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.linear").c_str(),     light->attenuation().linear);

    setUniformValue(formatUniformTableName("gSpotLights", index, "direction").c_str(),               light->direction());

    // Convert cutoff to radians
    setUniformValue(formatUniformTableName("gSpotLights", index, "cutoff").c_str(),                  cosf(qDegreesToRadians(light->angleOuterCone())));
}

void BasicLightning::setPointLight(const Entity::Light* light, int index)
{
    setUniformValue(formatUniformTableName("gPointLights", index, "base.color").c_str(),             linearColor(light->color()) * light->diffuseIntensity());
    setUniformValue(formatUniformTableName("gPointLights", index, "position").c_str(),               light->position());
    setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.constant").c_str(),   light->attenuation().constant);
    setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.exp").c_str(),        light->attenuation().quadratic);
    setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.linear").c_str(),     light->attenuation().linear);
}

void BasicLightning::setShadowEnabled(bool value)
{
    setUniformValue("gShadowsEnabled", value);
}

std::string BasicLightning::formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members) const
{
    std::stringstream ss;
    ss << table << '[' << index << ']';

    if(!members.empty())
        ss << '.' << members;

    return ss.str();
}