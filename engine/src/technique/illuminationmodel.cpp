#include "illuminationmodel.h"

#include "entity/light.h"
#include "mathelp.h"

#include <qmath.h>

using namespace Engine;
using namespace Engine::Technique;

IlluminationModel::IlluminationModel()
    : DSMaterialShader()
{
}

IlluminationModel::~IlluminationModel()
{
}


void IlluminationModel::setViewMatrix(const QMatrix4x4& mat)
{
    view_ = mat;
}

void IlluminationModel::enableSpotLight(const Entity::Light& light)
{
    useSubroutine("calculateOutput", "spotLightPass", GL_FRAGMENT_SHADER);

    setPointUniforms(light);
    setUniformValue("light.direction", view_.mapVector(light.direction()));

    // Convert cutoff angles to radians and precalculate cosine
    setUniformValue("light.outerAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleOuterCone()))));
    //setUniformValue("light.innerAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleInnerCone()))));
}

void IlluminationModel::enablePointLight(const Entity::Light& light)
{
    useSubroutine("calculateOutput", "pointLightPass", GL_FRAGMENT_SHADER);
    setPointUniforms(light);
}

void IlluminationModel::enableDirectionalLight(const Entity::Light& light)
{
    useSubroutine("calculateOutput", "directionalLightPass", GL_FRAGMENT_SHADER);

    float ambientFactor = 0.0f;
    if(light.diffuseIntensity() > 0.0f)
    {
        ambientFactor = light.ambientIntensity() / light.diffuseIntensity();
    }

    setUniformValue("light.ambientIntensity", ambientFactor);
    setUniformValue("light.color", linearColor(light.color()) * light.diffuseIntensity());
    setUniformValue("light.direction", view_.mapVector(light.direction()));
}

bool IlluminationModel::setLightningModel(const QString& name)
{
    lightningModel_ = name;
    if(!program()->isLinked())
    {
        return true;
    }

    return useSubroutine("lightningModel", name, GL_FRAGMENT_SHADER);
}

bool IlluminationModel::init()
{
    if(!DSMaterialShader::init())
    {
        return false;
    }

    if(resolveSubroutineLocation("pointLightPass", GL_FRAGMENT_SHADER) == GL_INVALID_INDEX)
        return false;

    if(resolveSubroutineLocation("spotLightPass", GL_FRAGMENT_SHADER) == GL_INVALID_INDEX)
        return false;

    if(resolveSubroutineLocation("directionalLightPass", GL_FRAGMENT_SHADER) == GL_INVALID_INDEX)
        return false;

    if(!lightningModel_.isEmpty())
    {
        useSubroutine("lightningModel", lightningModel_, GL_FRAGMENT_SHADER);
    }

    return true;
}

void IlluminationModel::setPointUniforms(const Entity::Light& spot)
{
    setUniformValue("light.color", linearColor(spot.color()) * spot.diffuseIntensity());
    setUniformValue("light.position", view_.mapVector(spot.position()));

    setUniformValue("light.attenuation.constant", spot.attenuation().constant);
    setUniformValue("light.attenuation.linear", spot.attenuation().linear);
    setUniformValue("light.attenuation.quadratic", spot.attenuation().quadratic);
}