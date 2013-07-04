#include "hdr.h"

#include <vector>

using namespace Engine;

Hdr::Hdr(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), fbo_(nullptr), downSampler_(funcs)
{
    // Tonemap program
    tonemap_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/passthrough.vert");
    tonemap_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tone.frag");
    tonemap_.link();

    // Highpass program
    highpass_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/passthrough.vert");
    highpass_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/highpass.frag");
    highpass_.link();

    null_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/passthrough.vert");
    null_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/null.frag");
    null_.link();
}

Hdr::~Hdr()
{
    if(fbo_ != nullptr)
        delete fbo_;
}

bool Hdr::initialize(int width, int height)
{
    if(fbo_ != nullptr)
        delete fbo_;

    fbo_ = new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::NoAttachment,
        GL_TEXTURE_2D, GL_RGBA16F);

    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glBindTexture(GL_TEXTURE_2D, 0);

    if(!fbo_->isValid())
        return false;

    return downSampler_.init(width, height, GL_RGBA16F);
}

void Hdr::render(GLuint vao, GLsizei size)
{
    if(inputTexture() == 0)
        return;

    gl->glBindVertexArray(vao);

    // Pass 1
    // Highpass filter
    renderHighpass(size);

    // Pass 2
    // Downsample and blur input
    downSampler_.downSample(fbo_->texture(), size);

    // Pass 3
    // Blend samples
    blendSamples(size);

    // Pass 4
    // Render tonemap
    renderTonemap(size);

    gl->glBindVertexArray(0);
}

void Hdr::renderHighpass(GLsizei size)
{
    fbo_->bind();
    gl->glClear(GL_COLOR_BUFFER_BIT);

    highpass_.bind();

    gl->glActiveTexture(0);
    gl->glBindTexture(GL_TEXTURE_2D, inputTexture());
    highpass_.setUniformValue("renderedTexture", 0);
    highpass_.setUniformValue("brightThreshold", 1.2f);

    gl->glDrawArrays(GL_TRIANGLES, 0, size);

    highpass_.release();
    fbo_->release();
}

void Hdr::blendSamples(GLsizei size)
{
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE);
    gl->glViewport(0, 0, fbo_->width(), fbo_->height());

    fbo_->bind();
    gl->glClear(GL_COLOR_BUFFER_BIT);

    null_.bind();
    null_.setUniformValue("text", 0);

    for(int i = 0; i < DownSampler::SAMPLES; ++i)
    {
        auto sample = downSampler_.getSample(i);

        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, sample->texture());

        gl->glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    fbo_->release();
    null_.release();
    gl->glDisable(GL_BLEND);
}

void Hdr::renderTonemap(GLsizei size)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, outputFbo());
    gl->glClear(GL_COLOR_BUFFER_BIT);

    tonemap_.bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, inputTexture());

    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());

    tonemap_.setUniformValue("renderedTexture", 0);
    tonemap_.setUniformValue("bloomTexture", 1);

    tonemap_.setUniformValue("exposure", 1.1f);
    tonemap_.setUniformValue("bloomFactor", 0.3f);
    tonemap_.setUniformValue("brightMax", 1.2f);

    // Draw quad
    gl->glDrawArrays(GL_TRIANGLES, 0, 6);

    tonemap_.release();
}