#include "shadowmap.h"
#include "common.h"

#include <QDebug>

#include "entity/camera.h"
#include "renderable/renderable.h"
#include "material.h"

using namespace Engine;
using namespace Engine::Technique;

ShadowMap::ShadowMap()
    : Technique()
{
}

ShadowMap::~ShadowMap()
{
    destroySpotLights();
    destroyLight(directionalLight_);
}

ShadowMap::LightData::LightData()
    : fbo(0)
{
}

//
// Initialisation methods
//

bool ShadowMap::initSpotLights(GLsizei width, GLsizei height, size_t count)
{
    destroySpotLights();

    // Reserve space
    spotLights_.resize(count);

    for(size_t i = 0; i < count; ++i)
    {
        if(!initLight(spotLights_[i], width, height))
            return false;
    }

    return true;
}

bool ShadowMap::initDirectionalLight(GLsizei width, GLsizei height)
{
    destroyLight(directionalLight_);

    if(width < 1 || height < 1)
        return false;

    return initLight(directionalLight_, width, height);
}

bool ShadowMap::initLight(LightData& light, GLsizei width, GLsizei height)
{
    // Initialize depth texture
    light.texture = std::make_shared<Texture2D>();
    if(!light.texture->create(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT))
    {
        return false;
    }

    light.texture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    light.texture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    light.texture->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    light.texture->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize framebuffer
    gl->glGenFramebuffers(1, &light.fbo);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, light.fbo);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texture->textureId(), 0);
    gl->glDrawBuffer(GL_NONE);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qWarning() << __FUNCTION__ << "Failed to init ShadowMap fbo!";
        return false;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gl->glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

size_t ShadowMap::numSpotLights() const
{
    return spotLights_.size();
}

//
// Render methods
//

void ShadowMap::renderSpotLight(size_t index, const Entity::Light* light, VisibleScene* visibles)
{
    // Calculate light frustrum
    const Texture2D::Ptr& texture = spotLights_.at(index).texture;
    QMatrix4x4& vp = spotLights_.at(index).worldView;

    vp.setToIdentity();
    vp.perspective(50.0f, static_cast<float>(texture->width()) / texture->height(), 1.0f, 150.0f);

    QMatrix4x4 look;
    look.lookAt(light->position(), light->position() + light->direction(), QVector3D(0, 1, 0));
    vp *= look;

    gl->glViewport(0, 0, texture->width(), texture->height());

    // Cull & render visibles
    renderLight(spotLights_.at(index), visibles);
}

void ShadowMap::renderDirectinalLight(Entity::Light* light, VisibleScene* visibles)
{
    if(light == nullptr)
        return;

    // Calculate light frustrum
    Entity::Camera view(QRectF(-95, -65, 190, 130), light->direction());
    view.setNearPlane(-80.0f);
    view.setFarPlane(400.0f);
    view.update();

    directionalLight_.worldView = view.worldView();

    gl->glViewport(0, 0, directionalLight_.texture->width(), directionalLight_.texture->height());

    // Render objects inside frustrum
    renderLight(directionalLight_, visibles);
}

void ShadowMap::renderLight(const LightData& light, VisibleScene* visibles)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, light.fbo);
    gl->glClear(GL_DEPTH_BUFFER_BIT);

    if(visibles == nullptr)
        return;

    program()->setUniformValue("gMaskSampler", 0);

    // Cull & render visibles
    RenderQueue shadowCasters;
    visibles->queryVisibles(light.worldView, shadowCasters, true);

    for(auto it = shadowCasters.begin(); it != shadowCasters.end(); ++it)
    {
        program()->setUniformValue("gMVP", light.worldView * *it->modelView);

        // Bind mask texture to discard fully opaque fragments
        it->material->getTexture(Material::TEXTURE_MASK)->bindActive(GL_TEXTURE0);
        it->renderable->render();
    }
}

//
// Binding and VP methods
//

bool ShadowMap::bindSpotLight(size_t index, GLenum textureUnit)
{
    return spotLights_.at(index).texture->bindActive(textureUnit);
}

bool ShadowMap::bindDirectionalLight(GLenum textureUnit)
{
    return directionalLight_.texture->bindActive(textureUnit);
}

const QMatrix4x4& ShadowMap::spotLightVP(size_t index) const
{
    return spotLights_.at(index).worldView;
}

const QMatrix4x4& ShadowMap::directionalLightVP() const
{
    return directionalLight_.worldView;
}

//
// Cleanup
//

void ShadowMap::destroySpotLights()
{
    for(auto it = spotLights_.begin(); it != spotLights_.end(); ++it)
    {
        destroyLight(*it);
    }

    spotLights_.clear();
}

void ShadowMap::destroyLight(LightData& light)
{
    if(light.fbo != 0)
    {
        gl->glDeleteFramebuffers(1, &light.fbo);
        light.fbo = 0;
    }

    if(light.texture != nullptr)
    {
        light.texture->release();
        light.texture.reset();
    }
}