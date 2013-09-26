#include "downsampler.h"

#include "resource/resourcedespatcher.h"

#include <QOpenGLFramebufferObjectFormat>

using namespace Engine;
using namespace Engine::Effect;

DownSampler::DownSampler(ResourceDespatcher* despatcher)
{
    fbos_.resize(SAMPLES, nullptr);

    program_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert")));
    program_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/blur.frag")));
}

DownSampler::~DownSampler()
{
    destroy();
}

void DownSampler::destroy()
{
    for(auto it = fbos_.begin(); it < fbos_.end(); ++it)
    {
        if(*it != nullptr)
        {
            delete *it;
            *it = nullptr;
        }
    }
}

bool DownSampler::init(int width, int height, GLenum format)
{
    destroy();

    for(size_t i = 0; i < fbos_.size(); ++i)
    {
        width = ceil(width / 2.0f);
        height = ceil(height / 2.0f);

        fbos_[i] = new QOpenGLFramebufferObject(width, height,
            QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D, format);

        if(!fbos_[i]->isValid())
            return false;

        gl->glBindTexture(GL_TEXTURE_2D, fbos_[i]->texture());
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    gl->glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool DownSampler::downSample(GLuint textureId, const Renderable::Quad& quad)
{
    if(!program_.ready())
        return false;

    program_->bind();

    gl->glActiveTexture(0);
    program_->setUniformValue("tex", 0);

    GLuint prevTexture = textureId;

    for(size_t i = 0; i < fbos_.size(); ++i)
    {
        fbos_[i]->bind();
        gl->glClear(GL_COLOR_BUFFER_BIT);

        program_->setUniformValue("width", fbos_[i]->width());
        program_->setUniformValue("height", fbos_[i]->height());

        gl->glBindTexture(GL_TEXTURE_2D, prevTexture);
        gl->glViewport(0, 0, fbos_[i]->width(), fbos_[i]->height());

        quad.renderDirect();

        fbos_[i]->release();
        prevTexture = fbos_[i]->texture();
    }

    program_->release();
    return true;
}

QOpenGLFramebufferObject* DownSampler::getSample(size_t n)
{
    return fbos_[n];
}