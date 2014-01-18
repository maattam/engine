//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "illuminationmodel.h"

#include "graph/light.h"
#include "graph/camera.h"
#include "shadowmap.h"
#include "mathelp.h"
#include "gbuffer.h"

#include <qmath.h>

using namespace Engine;
using namespace Engine::Technique;

IlluminationModel::IlluminationModel(unsigned int samples)
    : DSMaterialShader(samples), shadowUnit_(0)
{
}

IlluminationModel::~IlluminationModel()
{
}

void IlluminationModel::setQuadExtents(float scale, const QVector3D& center)
{
    setUniformValue("quadScale", scale);
    setUniformValue("quadCenter", center);
}

void IlluminationModel::setCamera(const Graph::Camera& camera)
{
    setUniformValue("viewProj", camera.worldView());
    setUniformValue("cameraUp", camera.up());
    setUniformValue("cameraRight", camera.right());

    view_ = camera.view();
    setUniformValue("viewInverse", view_.inverted());
}

void IlluminationModel::enableSpotLight(const Graph::Light& light, ShadowMap* shadow)
{
    useSubroutine("calculateOutput", "spotLightPass", GL_FRAGMENT_SHADER);

    setPointUniforms(light);
    setUniformValue("light.direction", view_.mapVector(light.direction()));

    // Convert cutoff angles to radians and precalculate cosine
    setUniformValue("light.cosOuterAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleOuterCone()))));
    setUniformValue("light.cosInnerAngle", static_cast<float>(qCos(qDegreesToRadians(light.angleInnerCone()))));

    if(shadow != nullptr)
    {
        setUniformValue("shadowEnabled", true);
        setUniformValue("lightVP", shadow->lightVP());
        setUniformValue("shadowOffset", QVector2D(1.0 / shadow->size().width(), 1.0 / shadow->size().height()));

        shadow->bindTextures(GL_TEXTURE0 + shadowUnit_);
    }

    else
    {
        setUniformValue("shadowEnabled", false);
    }
}

void IlluminationModel::enablePointLight(const Graph::Light& light)
{
    useSubroutine("calculateOutput", "pointLightPass", GL_FRAGMENT_SHADER);
    setPointUniforms(light);
}

void IlluminationModel::enableDirectionalLight(const Graph::Light& light)
{
    useSubroutine("calculateOutput", "directionalLightPass", GL_FRAGMENT_SHADER);
    useSubroutine("transformQuad", "fullscreenQuad", GL_VERTEX_SHADER);

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

    // Bind samplers after last gbuffer unit
    shadowUnit_ = gbuffer()->textures().count();

    setUniformValue("shadowSampler", shadowUnit_);
    setUniformValue("shadowEnabled", false);

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