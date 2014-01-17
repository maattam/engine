//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "illuminationmodel.h"

#include "graph/light.h"
#include "mathelp.h"

#include <qmath.h>

using namespace Engine;
using namespace Engine::Technique;

IlluminationModel::IlluminationModel(unsigned int samples)
    : DSMaterialShader(samples)
{
}

IlluminationModel::~IlluminationModel()
{
}


void IlluminationModel::setViewMatrix(const QMatrix4x4& mat)
{
    view_ = mat;
}

void IlluminationModel::setCameraAxes(const QVector3D& up, const QVector3D& right)
{
    setUniformValue("cameraUp", up);
    setUniformValue("cameraRight", right);
}

void IlluminationModel::setQuadExtents(float scale, const QVector3D& center)
{
    setUniformValue("quadScale", scale);
    setUniformValue("quadCenter", center);
}

void IlluminationModel::setViewProjMatrix(const QMatrix4x4& mat)
{
    setUniformValue("viewProj", mat);
}

void IlluminationModel::enableSpotLight(const Graph::Light& light)
{
    useSubroutine("calculateOutput", "spotLightPass", GL_FRAGMENT_SHADER);

    setPointUniforms(light);
    setUniformValue("light.direction", view_.mapVector(light.direction()));

    // Convert cutoff angles to radians and precalculate cosine
    setUniformValue("light.cosOuterAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleOuterCone()))));
    setUniformValue("light.cosInnerAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleInnerCone()))));
}

void IlluminationModel::enablePointLight(const Graph::Light& light)
{
    useSubroutine("calculateOutput", "pointLightPass", GL_FRAGMENT_SHADER);
    setPointUniforms(light);
}

void IlluminationModel::enableDirectionalLight(const Graph::Light& light)
{
    useSubroutine("calculateOutput", "directionalLightPass", GL_FRAGMENT_SHADER);
    useSubroutine( "transformQuad", "fullscreenQuad", GL_VERTEX_SHADER);

    float ambientFactor = 0.0f;
    if(light.diffuseIntensity() > 0.0f)
    {
        ambientFactor = light.ambientIntensity() / light.diffuseIntensity();
    }

    setUniformValue("light.ambientIntensity", ambientFactor);
    setUniformValue("light.color", linearColor(light.color()) * light.diffuseIntensity());
    setUniformValue("light.direction", view_.mapVector(light.direction()));
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

    if(resolveSubroutineLocation("screenOrientedQuad", GL_VERTEX_SHADER) == GL_INVALID_INDEX)
        return false;

    if(resolveSubroutineLocation("fullscreenQuad", GL_VERTEX_SHADER) == GL_INVALID_INDEX)
        return false;

    return true;
}

void IlluminationModel::setPointUniforms(const Graph::Light& spot)
{
    useSubroutine( "transformQuad", "screenOrientedQuad", GL_VERTEX_SHADER);

    setUniformValue("light.color", linearColor(spot.color()) * spot.diffuseIntensity());
    setUniformValue("light.position", view_ * spot.position());

    QVector3D attn;
    attn.setX(spot.attenuation().constant);
    attn.setY(spot.attenuation().linear);
    attn.setZ(spot.attenuation().quadratic);

    setUniformValue("light.attenuation", attn);
}