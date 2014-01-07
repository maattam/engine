#include "shadowmap.h"
#include "common.h"

#include <QDebug>

#include "graph/camera.h"
#include "renderable/renderable.h"
#include "material.h"
#include "renderqueue.h"
#include "binder.h"

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

bool ShadowMap::init()
{
    return true;
}

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
    if(!light.texture->create(0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT))
    {
        return false;
    }

    light.texture->setFiltering(GL_LINEAR, GL_LINEAR);
    light.texture->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    const float color[4] = { 1.0f };
    gl->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    // Nvidia PCF shadow2DProj
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // Initialize framebuffer
    gl->glGenFramebuffers(1, &light.fbo);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, light.fbo);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texture->handle(), 0);
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

void ShadowMap::renderSpotLight(size_t index, const Graph::Light* light, RenderQueue* visibles)
{
    const auto& texture = spotLights_.at(index).texture;

    // Calculate light frustrum
    QMatrix4x4 view;
    view.perspective(light->angleOuterCone() * 2, 1.0f, 1.0f, light->cutoffDistance());
    view.lookAt(light->position(), light->position() + light->direction(), UNIT_Y);

    // Update view
    spotLights_.at(index).worldView = view;

    gl->glViewport(0, 0, texture->width(), texture->height());

    // Cull & render visibles
    renderLight(spotLights_.at(index), visibles);
}

void ShadowMap::renderDirectinalLight(Graph::Light* light, RenderQueue* visibles)
{
    if(light == nullptr)
        return;

    // Calculate light frustrum
    Graph::Camera view(QRectF(-95, -65, 190, 130), light->direction());
    view.setNearPlane(-80.0f);
    view.setFarPlane(1000.0f);
    view.update();

    directionalLight_.worldView = view.worldView();

    gl->glViewport(0, 0, directionalLight_.texture->width(), directionalLight_.texture->height());

    // Render objects inside frustrum
    renderLight(directionalLight_, visibles);
}

void ShadowMap::renderLight(const LightData& light, RenderQueue* visibles)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, light.fbo);
    gl->glClear(GL_DEPTH_BUFFER_BIT);

    if(visibles->queue().empty())
    {
        return;
    }

    setUniformValue("gMaskSampler", 0);

    for(auto it = visibles->begin(); it != visibles->end(); ++it)
    {
        setUniformValue("gMVP", light.worldView * *it->modelView);

        // Bind mask texture to discard fully opaque fragments
        Binder::bind(it->material->getTexture(Material::TEXTURE_MASK), GL_TEXTURE0);
        it->renderable->render();
    }
}

//
// Binding and VP methods
//

bool ShadowMap::bindSpotLight(size_t index, GLenum textureUnit)
{
    return spotLights_.at(index).texture->bind(textureUnit);
}

bool ShadowMap::bindDirectionalLight(GLenum textureUnit)
{
    return directionalLight_.texture->bind(textureUnit);
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
        light.texture->remove();
        light.texture.reset();
    }
}