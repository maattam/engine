#include "shadowmap.h"
#include "common.h"

#include <QDebug>
#include <cassert>

#include "resourcedespatcher.h"
#include "shader.h"
#include "entity/camera.h"

using namespace Engine;
using namespace Engine::Technique;

ShadowMap::ShadowMap(ResourceDespatcher* despatcher)
    : Technique()
{
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/shadowmap.vert"), Shader::Type::Vertex));
}

ShadowMap::~ShadowMap()
{
    destroySpotLights();
    destroyDirectionalLight();
}

bool ShadowMap::initSpotLights(GLsizei width, GLsizei height, size_t count)
{
    destroySpotLights();

    // Reserve space
    spotLightFbos_.resize(count, 0);
    spotLightTextures_.resize(count);
    spotLightVPs_.resize(count);

    // Generate fbos/textures
    gl->glGenFramebuffers(count, &spotLightFbos_[0]);

    for(size_t i = 0; i < count; ++i)
    {
        spotLightTextures_[i] = std::make_shared<Texture2D>();
        if(!initDepthFBO(spotLightFbos_[i], *spotLightTextures_[i], width, height))
            return false;
    }

    return true;
}

void ShadowMap::enableSpotLight(size_t index, const Entity::VisibleLight& light)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, spotLightFbos_.at(index));

    const Texture2D::Ptr& texture = spotLightTextures_.at(index);

    // Calculate world matrix
    QMatrix4x4& vp = spotLightVPs_.at(index);

    vp.setToIdentity();
    vp.perspective(50.0f, static_cast<float>(texture->width()) / texture->height(), 1.0f, 150.0f);

    QMatrix4x4 look;
    look.lookAt(light.first, light.first + light.second->direction, QVector3D(0, 1, 0));
    vp *= look;

    gl->glViewport(0, 0, texture->width(), texture->height());
}

void ShadowMap::bindSpotLight(size_t index, GLenum textureUnit)
{
    spotLightTextures_.at(index)->bindActive(textureUnit);
}

void ShadowMap::setLightMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue("gMVP", mvp);
}

void ShadowMap::destroySpotLights()
{
    if(spotLightFbos_.size() > 0)
    {
        gl->glDeleteFramebuffers(spotLightFbos_.size(), &spotLightFbos_[0]);
        spotLightFbos_.clear();
    }
    
    spotLightTextures_.clear();
    spotLightVPs_.clear();
}

const QMatrix4x4& ShadowMap::spotLightVP(size_t index) const
{
    assert(index < spotLightVPs_.size());
    return spotLightVPs_.at(index);
}

bool ShadowMap::initDirectionalLight(GLsizei width, GLsizei height)
{
    destroyDirectionalLight();

    if(width < 1 || height < 1)
        return false;

    gl->glGenFramebuffers(1, &directionalLightFbo_);

    return initDepthFBO(directionalLightFbo_, directionalLightTexture_, width, height);
}

void ShadowMap::enableDirectinalLight(Entity::Light* light)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, directionalLightFbo_);

    QVector3D direction = light->direction;
    direction.normalize();

    Entity::Camera view(QRectF(-95, -65, 190, 130), direction);
    view.setNearPlane(-80.0f);
    view.setFarPlane(400.0f);
    view.update();

    directionalLightVP_ = view.worldView();

    gl->glViewport(0, 0, directionalLightTexture_.width(), directionalLightTexture_.height());
}

void ShadowMap::bindDirectionalLight(GLenum textureUnit)
{
    directionalLightTexture_.bindActive(textureUnit);
}

const QMatrix4x4& ShadowMap::directionalLightVP() const
{
    return directionalLightVP_;
}

void ShadowMap::destroyDirectionalLight()
{
    if(directionalLightFbo_ != 0)
    {
        gl->glDeleteFramebuffers(1, &directionalLightFbo_);
        directionalLightFbo_ = 0;
    }

    directionalLightTexture_.release();
}

bool ShadowMap::initDepthFBO(GLuint fbo, Texture2D& texture, GLsizei width, GLsizei height)
{
    // Initialize depth texture
    if(!texture.create(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT))
    {
        return false;
    }

    texture.texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture.texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture.texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.textureId(), 0);
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