#include "shadowmap.h"

#include <QDebug>

#include <cassert>

using namespace Engine;

ShadowMapTechnique::ShadowMapTechnique(QOpenGLFunctions_4_2_Core* funcs)
    : Technique(), gl(funcs)
{
    // Compile and link program
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shadowmap.vert");
    program_.link();
}

ShadowMapTechnique::~ShadowMapTechnique()
{
    destroySpotLights();
}

bool ShadowMapTechnique::initSpotLights(unsigned int width, unsigned int height, size_t count)
{
    destroySpotLights();

    if(!program_.isLinked())
        return false;

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
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Initialize framebuffer
        gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, spotLightFbos_[i]);
        gl->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotLightTextures_[i], 0);
        gl->glDrawBuffer(GL_NONE);

        if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            qDebug() << "Failed to init ShadowMapTechnique fbo!";
            return false;
        }
    }

    return true;
}

void ShadowMapTechnique::enableSpotLight(size_t index, const SpotLight& light)
{
    assert(index < spotLightFbos_.size());

    gl->glBindFramebuffer(GL_FRAMEBUFFER, spotLightFbos_.at(index));

    // Calculate world matrix
    QMatrix4x4& vp = spotLightVPs_.at(index);
    vp.setToIdentity();
    vp.perspective(30.0f, static_cast<float>(spotWidth_) / spotHeight_, 1.0f, 50.0f);

    QMatrix4x4 look;
    look.lookAt(light.position, light.position + light.direction, QVector3D(0, 1, 0));
    vp *= look;
}

void ShadowMapTechnique::bindSpotLight(size_t index, GLenum textureUnit)
{
    assert(index < spotLightTextures_.size());

    gl->glActiveTexture(textureUnit);
    gl->glBindTexture(GL_TEXTURE_2D, spotLightTextures_.at(index));
}

void ShadowMapTechnique::setLightMVP(const QMatrix4x4& mvp)
{
    program_.setUniformValue("gMVP", mvp);
}

void ShadowMapTechnique::destroySpotLights()
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

const QMatrix4x4& ShadowMapTechnique::spotLightVP(size_t index) const
{
    assert(index < spotLightVPs_.size());
    return spotLightVPs_.at(index);
}