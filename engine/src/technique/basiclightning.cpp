#include "basiclightning.h"

#include <QDebug>
#include <qmath.h>
#include <QTime>

#include <sstream>
#include <cassert>

using namespace Engine;
using namespace Engine::Technique;

namespace {
    // Maps colors from srgb space with gamma of 2.2 to linear space
    QVector3D linear(const QVector3D& color);
}

BasicLightning::BasicLightning()
    : Technique(), mvpLocation_(-1)
{
}

BasicLightning::~BasicLightning()
{
}

void BasicLightning::init()
{
    mvpLocation_ = program()->uniformLocation("gMVP");
}

void BasicLightning::setMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue(mvpLocation_, mvp);
}

void BasicLightning::setWorldView(const QMatrix4x4& vp)
{
    program()->setUniformValue("gWorld", vp);
}

void BasicLightning::setEyeWorldPos(const QVector3D& eyePos)
{
    program()->setUniformValue("gEyeWorldPos", eyePos);
}

void BasicLightning::setHasTangents(bool tangents)
{
    program()->setUniformValue("gHasTangents", tangents);
}

void BasicLightning::setMaterialAttributes(const Material::Attributes& attributes)
{
    program()->setUniformValue("gMaterial.ambientColor", linear(attributes.ambientColor));
    program()->setUniformValue("gMaterial.diffuseColor", linear(attributes.diffuseColor));
    program()->setUniformValue("gMaterial.specularIntensity", attributes.specularIntensity);
    program()->setUniformValue("gMaterial.shininess", attributes.shininess);
}

void BasicLightning::setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular, GLuint mask)
{
    program()->setUniformValue("gDiffuseSampler", diffuse);
    program()->setUniformValue("gNormalSampler", normal);
    program()->setUniformValue("gSpecularSampler", specular);
    program()->setUniformValue("gMaskSampler", mask);
}

void BasicLightning::setDirectionalLight(Entity::Light* light)
{
    float ambientFactor = 0.0f;
    if(light->diffuseIntensity() > 0.0f)
    {
        ambientFactor = light->ambientIntensity() / light->diffuseIntensity();
    }

    program()->setUniformValue("gDirectionalLight.ambientIntensity", ambientFactor);
    program()->setUniformValue("gDirectionalLight.base.color", linear(light->color()) * light->diffuseIntensity());

    program()->setUniformValue("gDirectionalLight.direction", light->direction());
}

void BasicLightning::setSpotLightMVP(size_t index, const QMatrix4x4& mvp)
{
    assert(index < MAX_SPOT_LIGHTS);
    program()->setUniformValue(formatUniformTableName("gLightMVP", index).c_str(), mvp);
}

void BasicLightning::setSpotLightShadowUnit(size_t index, GLuint shadow)
{
     assert(index < MAX_SPOT_LIGHTS);
     program()->setUniformValue(formatUniformTableName("gSpotLightShadowMap", index).c_str(), shadow);
}

void BasicLightning::setDirectionalLightMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue("gDirectionalLightMVP", mvp);
}

void BasicLightning::setDirectionalLightShadowUnit(GLuint shadow)
{
    program()->setUniformValue("gDirectionalLightShadowMap", shadow);
}

void BasicLightning::setPointAndSpotLights(const VisibleScene::Lights& lights)
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

    program()->setUniformValue("gNumPointLights", numPointLights);
    program()->setUniformValue("gNumSpotLights", numSpotLights);
}

void BasicLightning::setSpotLight(const Entity::Light* light, int index)
{
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "base.base.color").c_str(),             linear(light->color()) * light->diffuseIntensity());
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "base.position").c_str(),               light->position());
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.constant").c_str(),   light->attenuation().constant);
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.exp").c_str(),        light->attenuation().exp);
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "base.attenuation.linear").c_str(),     light->attenuation().linear);

    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "direction").c_str(),               light->direction());

    // Convert cutoff to radians
    program()->setUniformValue(formatUniformTableName("gSpotLights", index, "cutoff").c_str(),                  cosf(qDegreesToRadians(light->cutoff())));
}

void BasicLightning::setPointLight(const Entity::Light* light, int index)
{
    program()->setUniformValue(formatUniformTableName("gPointLights", index, "base.color").c_str(),             linear(light->color()) * light->diffuseIntensity());
    program()->setUniformValue(formatUniformTableName("gPointLights", index, "position").c_str(),               light->position());
    program()->setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.constant").c_str(),   light->attenuation().constant);
    program()->setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.exp").c_str(),        light->attenuation().exp);
    program()->setUniformValue(formatUniformTableName("gPointLights", index, "attenuation.linear").c_str(),     light->attenuation().linear);
}

void BasicLightning::setShadowEnabled(bool value)
{
    program()->setUniformValue("gShadowsEnabled", value);
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

namespace {

    QVector3D linear(const QVector3D& color)
    {
        const float gamma = 2.2f;

        return QVector3D(qPow(color.x(), gamma), qPow(color.y(), gamma), qPow(color.z(), gamma));
    }

}