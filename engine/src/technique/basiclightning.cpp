#include "basiclightning.h"

#include "resource/resourcedespatcher.h"
#include "resource/shader.h"

#include <QDebug>
#include <qmath.h>

#include <sstream>
#include <cassert>

using namespace Engine;
using namespace Engine::Technique;

BasicLightning::BasicLightning(ResourceDespatcher* despatcher)
    : Technique(), mvpLocation_(-1)
{
    addShader(despatcher->get<Shader>("shaders/basiclightning.vert"));
    addShader(despatcher->get<Shader>("shaders/basiclightning.frag"));
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
    program()->setUniformValue("gMaterial.ambientColor", attributes.ambientColor);
    program()->setUniformValue("gMaterial.diffuseColor", attributes.diffuseColor);
    program()->setUniformValue("gMaterial.specularIntensity", attributes.specularIntensity);
    program()->setUniformValue("gMaterial.shininess", attributes.shininess);
}

void BasicLightning::setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular)
{
    program()->setUniformValue("gDiffuseSampler", diffuse);
    program()->setUniformValue("gNormalSampler", normal);
    program()->setUniformValue("gSpecularSampler", specular);
}

void BasicLightning::setDirectionalLight(const Entity::DirectionalLight& light)
{
    program()->setUniformValue("gDirectionalLight.base.ambientIntensity", light.ambientIntensity);
    program()->setUniformValue("gDirectionalLight.base.color", light.color);
    program()->setUniformValue("gDirectionalLight.base.diffuseIntensity", light.diffuseIntensity);

    QVector3D direction = light.direction;
    direction.normalize();
    program()->setUniformValue("gDirectionalLight.direction", direction);
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

void BasicLightning::setPointLights(const std::vector<Entity::PointLight>& lights)
{
    unsigned int numLights = lights.size();
    if(numLights > MAX_POINT_LIGHTS)
    {
        qWarning() << __FUNCTION__ << "MAX_POINT_LIGHTS exceeded";
        numLights = MAX_POINT_LIGHTS;
    }

    program()->setUniformValue("gNumPointLights", numLights);

    for(size_t i = 0; i < numLights; ++i)
    {
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "base.ambientIntensity").c_str(),  lights[i].ambientIntensity);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "base.color").c_str(),             lights[i].color);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "base.diffuseIntensity").c_str(),  lights[i].diffuseIntensity);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "position").c_str(),               lights[i].position);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.constant").c_str(),   lights[i].attenuation.constant);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.exp").c_str(),        lights[i].attenuation.exp);
        program()->setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.linear").c_str(),     lights[i].attenuation.linear);
    }
}

void BasicLightning::setSpotLights(const std::vector<Entity::SpotLight>& lights)
{
    unsigned int numLights = lights.size();
    if(numLights > MAX_SPOT_LIGHTS)
    {
        qWarning() << __FUNCTION__ << "Warning: MAX_SPOT_LIGHTS exceeded";
        numLights = MAX_SPOT_LIGHTS;
    }

    program()->setUniformValue("gNumSpotLights", numLights);

    for(size_t i = 0; i < numLights; ++i)
    {
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.ambientIntensity").c_str(),  lights[i].ambientIntensity);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.color").c_str(),             lights[i].color);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.diffuseIntensity").c_str(),  lights[i].diffuseIntensity);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.position").c_str(),               lights[i].position);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.constant").c_str(),   lights[i].attenuation.constant);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.exp").c_str(),        lights[i].attenuation.exp);
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.linear").c_str(),     lights[i].attenuation.linear);

        QVector3D direction = lights[i].direction;
        direction.normalize();

        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "direction").c_str(),     direction);

        // Convert cutoff to radians
        program()->setUniformValue(formatUniformTableName("gSpotLights", i, "cutoff").c_str(),        cosf(M_PI * lights[i].cutoff / 180));
    }
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