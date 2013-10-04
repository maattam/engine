#include "hdr.h"

#include "resourcedespatcher.h"

#include <vector>

using namespace Engine;
using namespace Engine::Effect;

Hdr::Hdr(ResourceDespatcher* despatcher, int bloomLevels)
    : fbo_(nullptr), samples_(1), downSampler_(despatcher), width_(0), height_(0), bloomLevels_(bloomLevels)
{
    // Tonemap program
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert")));
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/tone.frag")));

    // Highpass program
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert")));
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/highpass.frag")));
}

Hdr::~Hdr()
{
    if(fbo_ != nullptr)
        delete fbo_;
}

bool Hdr::initialize(int width, int height, int samples)
{
    if(bloomLevels_ < 1 || width < 1 || height < 1)
        return false;

    if(fbo_ != nullptr)
        delete fbo_;

    if(samples <= 0)
        samples = 1;

    width_ = width;
    height_ = height;

    //width = ceil(width / 2.0f);
    //height = ceil(height / 2.0f);

    samples_ = samples;

    QOpenGLFramebufferObjectFormat format;
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setMipmap(true);                     // Allocate mipmaps for downsampling
    format.setInternalTextureFormat(GL_RGBA16F);
    format.setAttachment(QOpenGLFramebufferObject::NoAttachment);

    fbo_ = new QOpenGLFramebufferObject(width, height, format);

    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    gl->glBindTexture(GL_TEXTURE_2D, 0);

    if(!fbo_->isValid())
        return false;

    return downSampler_.init(width, height, fbo_->texture(), bloomLevels_);
}

void Hdr::render(const Renderable::Quad& quad)
{
    if(inputTexture() == 0)
        return;

    else if(!tonemap_.ready() || !highpass_.ready())
        return;

    quad.bindVaoDirect();

    // Pass 1
    // Highpass filter
    renderHighpass(quad);

    // Pass 2
    // Downsample and blur input
    if(!downSampler_.downSample(fbo_->texture(), quad))
    {
        return;
    }

    // Pass 3
    // Render tonemap to output
    renderTonemap(quad);

    gl->glBindVertexArray(0);
}

void Hdr::renderHighpass(const Renderable::Quad& quad)
{
    fbo_->bind();
    gl->glClear(GL_COLOR_BUFFER_BIT);

    highpass_->bind();
    gl->glViewport(0, 0, fbo_->width(), fbo_->height());

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, inputTexture());

    highpass_->setUniformValue("renderedTexture", 0);
    highpass_->setUniformValue("threshold", 1.1f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    fbo_->release();

    // Generate mipmaps for fbo
    //gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    //gl->glGenerateMipmap(GL_TEXTURE_2D);
}

void Hdr::renderTonemap(const Renderable::Quad& quad)
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, outputFbo());
    gl->glClear(GL_COLOR_BUFFER_BIT);

    gl->glViewport(0, 0, width_, height_);

    tonemap_->bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, inputTexture());
    tonemap_->setUniformValue("renderedTexture", 0);

    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    tonemap_->setUniformValue("bloomSampler", 1);
    tonemap_->setUniformValue("bloomLevels", bloomLevels_);

    tonemap_->setUniformValue("samples", samples_);
    tonemap_->setUniformValue("exposure", 1.1f);
    tonemap_->setUniformValue("bloomFactor", 0.2f);
    tonemap_->setUniformValue("brightMax", 1.1f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}