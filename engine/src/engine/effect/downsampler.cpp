#include "downsampler.h"

#include <QOpenGLFramebufferObjectFormat>

using namespace Engine;

DownSampler::DownSampler(QOpenGLFunctions_4_2_Core* funcs) : gl(funcs)
{
    fbos_.resize(SAMPLES, nullptr);

    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/passthrough.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blur.frag");
    program_.link();
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

    for(int i = 0; i < fbos_.size(); ++i)
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

void DownSampler::downSample(GLuint textureId, GLuint size)
{
    gl->glEnable(GL_TEXTURE_2D);
    program_.bind();

    gl->glActiveTexture(0);
    program_.setUniformValue("tex", 0);

    GLuint prevTexture = textureId;

    for(int i = 0; i < fbos_.size(); ++i)
    {
        fbos_[i]->bind();
        gl->glClear(GL_COLOR_BUFFER_BIT);

        program_.setUniformValue("width", fbos_[i]->width());
        program_.setUniformValue("height", fbos_[i]->height());

        gl->glBindTexture(GL_TEXTURE_2D, prevTexture);
        gl->glViewport(0, 0, fbos_[i]->width(), fbos_[i]->height());

        gl->glDrawArrays(GL_TRIANGLES, 0, size);

        fbos_[i]->release();
        prevTexture = fbos_[i]->texture();
    }

    program_.release();
}

QOpenGLFramebufferObject* DownSampler::getSample(size_t n)
{
    return fbos_[n];
}