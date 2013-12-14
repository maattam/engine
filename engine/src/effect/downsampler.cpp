#include "downsampler.h"

#include "resourcedespatcher.h"

#include <QOpenGLFramebufferObjectFormat>

using namespace Engine;
using namespace Engine::Effect;

DownSampler::DownSampler(ResourceDespatcher* despatcher)
    : width_(0), height_(0)
{
    program_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    program_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/blur.frag"), Shader::Type::Fragment));
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

bool DownSampler::init(int width, int height, GLuint texture, GLuint maxLod)
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
    if(!program_.ready())
        return false;

    program_->bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, textureId);
    program_->setUniformValue("tex", 0);

    int width = width_;
    int height = height_;

    for(int i = 0; i < fbos_.size(); ++i)
    {
        width = floor(width / 2.0f);
        height = floor(height / 2.0f);

        gl->glBindFramebuffer(GL_FRAMEBUFFER, fbos_[i]);

        program_->setUniformValue("width", width);
        program_->setUniformValue("height", height);
        program_->setUniformValue("lodLevel", static_cast<float>(i));

        gl->glViewport(0, 0, width, height);

        quad.renderDirect();
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    program_->release();

    return true;
}