#include "shadowmap.h"
#include "common.h"

#include <QDebug>
#include <cassert>

#include "resource/resourcedespatcher.h"
#include "resource/shader.h"

using namespace Engine;
using namespace Engine::Technique;

ShadowMap::ShadowMap(ResourceDespatcher* despatcher)
    : Technique()
{
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/shadowmap.vert")));
}

ShadowMap::~ShadowMap()
{
    destroySpotLights();
}

bool ShadowMap::initSpotLights(unsigned int width, unsigned int height, size_t count)
{
    destroySpotLights();

    spotWidth_ = width;
    spotHeight_ = height;

    // Reserve space
    spotLightFbos_.resize(count, 0);
    spotLightTextures_.resize(count, 0);
    spotLightVPs_.resize(count);

    // Generate fbos/textures
    gl->glGenTextures(count, &spotLightTextures_[0]);
    gl->glGenFramebuffers(count, &spotLightFbos_[0]);

    for(size_t i = 0; i < count; ++i)
    {
        // Initialize depth texture
        gl->glBindTexture(GL_TEXTURE_2D, spotLightTextures_[i]);
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Initialize framebuffer
        gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, spotLightFbos_[i]);
        gl->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotLightTextures_[i], 0);
        gl->glDrawBuffer(GL_NONE);

        if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            qWarning() << __FUNCTION__ << "Failed to init ShadowMap fbo!";
            return false;
        }
    }

    return true;
}

void ShadowMap::enableSpotLight(size_t index, const Entity::SpotLight& light)
{
    assert(index < spotLightFbos_.size());

    gl->glBindFramebuffer(GL_FRAMEBUFFER, spotLightFbos_.at(index));

    // Calculate world matrix
    QMatrix4x4& vp = spotLightVPs_.at(index);
    vp.setToIdentity();
    vp.perspective(50.0f, static_cast<float>(spotWidth_) / spotHeight_, 1.0f, 150.0f);

    QMatrix4x4 look;
    look.lookAt(light.position, light.position + light.direction, QVector3D(0, 1, 0));
    vp *= look;
}

void ShadowMap::bindSpotLight(size_t index, GLenum textureUnit)
{
    assert(index < spotLightTextures_.size());

    gl->glActiveTexture(textureUnit);
    gl->glBindTexture(GL_TEXTURE_2D, spotLightTextures_.at(index));
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

    if(spotLightTextures_.size() > 0)
    {
        gl->glDeleteTextures(spotLightTextures_.size(), &spotLightTextures_[0]);
        spotLightTextures_.clear();
    }

    spotLightVPs_.clear();
}

const QMatrix4x4& ShadowMap::spotLightVP(size_t index) const
{
    assert(index < spotLightVPs_.size());
    return spotLightVPs_.at(index);
}