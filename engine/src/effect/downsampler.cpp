#include "downsampler.h"

#include "resourcedespatcher.h"
#include "technique/blurfilter.h"

using namespace Engine;
using namespace Engine::Effect;

DownSampler::DownSampler(ResourceDespatcher* despatcher)
    : width_(0), height_(0)
{
}

DownSampler::~DownSampler()
{
    destroy();
}

void DownSampler::destroy()
{
    for(auto it = fbos_.begin(); it < fbos_.end(); ++it)
    {
        gl->glDeleteFramebuffers(fbos_.size(), &fbos_[0]);
    }

    fbos_.clear();
}

void DownSampler::setBlurFilter(const FilterPtr& filter)
{
    filter_ = filter;
    filter_->setTextureUnit(0);
}

bool DownSampler::initialise(int width, int height, GLuint texture, GLuint maxLod)
{
    destroy();

    if(maxLod < 1)
    {
        return false;
    }

    width_ = width;
    height_ = height;

    fbos_.resize(maxLod, 0);

    gl->glGenFramebuffers(maxLod, &fbos_[0]);
    gl->glBindTexture(GL_TEXTURE_2D, texture);

    for(size_t i = 0; i < fbos_.size(); ++i)
    {
        gl->glBindFramebuffer(GL_FRAMEBUFFER, fbos_[i]);

        // Bind mipmap level to fbo
        gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, i+1);
        if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
    }

    return true;
}

bool DownSampler::downSample(GLuint textureId, const Renderable::Quad& quad)
{
    if(filter_ == nullptr || !filter_->enable())
    {
        return false;
    }

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, textureId);

    int width = width_;
    int height = height_;

    for(int i = 0; i < fbos_.size(); ++i)
    {
        width = floor(width / 2.0f);
        height = floor(height / 2.0f);

        gl->glBindFramebuffer(GL_FRAMEBUFFER, fbos_[i]);

        filter_->setTextureParams(width, height, static_cast<float>(i));

        gl->glViewport(0, 0, width, height);

        quad.renderDirect();
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}