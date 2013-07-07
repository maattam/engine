#include "hdr.h"

#include <vector>

using namespace Engine;

Hdr::Hdr(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), fbo_(nullptr), downSampler_(funcs), samples_(1)
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

bool Hdr::initialize(int width, int height, int samples)
{
    if(fbo_ != nullptr)
        delete fbo_;

    if(samples <= 0)
        samples = 1;

    samples_ = samples;

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

void Hdr::render(const Quad& quad)
{
    if(inputTexture() == 0)
        return;

    quad.bindVaoDirect();

    // Pass 1
    // Highpass filter
    renderHighpass(quad);

    // Pass 2
    // Downsample and blur input
    downSampler_.downSample(fbo_->texture(), quad);

    // Pass 3
    // Render tonemap to output
    renderTonemap(quad);

    gl->glBindVertexArray(0);
}

void Hdr::renderHighpass(const Quad& quad)
{
    fbo_->bind();
    gl->glClear(GL_COLOR_BUFFER_BIT);

    highpass_.bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, inputTexture());

    highpass_.setUniformValue("renderedTexture", 0);
    highpass_.setUniformValue("brightThreshold", 1.2f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    fbo_->release();
}

void Hdr::renderTonemap(const Quad& quad)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, outputFbo());
    gl->glClear(GL_COLOR_BUFFER_BIT);

    gl->glViewport(0, 0, fbo_->width(), fbo_->height());

    tonemap_.bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, inputTexture());
    tonemap_.setUniformValue("renderedTexture", 0);

    int bloomSampleId[DownSampler::SAMPLES] = {0};

    for(int i = 0; i < DownSampler::SAMPLES; ++i)
    {
        gl->glActiveTexture(GL_TEXTURE1 + i);
        gl->glBindTexture(GL_TEXTURE_2D, downSampler_.getSample(i)->texture());
        bloomSampleId[i] = i + 1;
    }

    tonemap_.setUniformValueArray("bloomSamplers", bloomSampleId, DownSampler::SAMPLES);

    tonemap_.setUniformValue("samples", samples_);
    tonemap_.setUniformValue("exposure", 1.1f);
    tonemap_.setUniformValue("bloomFactor", 0.3f);
    tonemap_.setUniformValue("brightMax", 1.2f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}