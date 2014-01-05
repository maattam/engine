#include "hdr.h"

#include "resourcedespatcher.h"
#include "technique/blurfilter.h"
#include "samplerfunction.h"
#include "technique/hdrtonemap.h"
#include "renderable/primitive.h"

#include <QOpenGLFramebufferObject>

using namespace Engine;
using namespace Engine::Effect;

Hdr::Hdr(ResourceDespatcher* despatcher, int bloomLevels)
    : fbo_(nullptr), downSampler_(despatcher), threshold_(1.0f),
    width_(0), height_(0), bloomLevels_(bloomLevels), sampleLevel_(0),
    quad_(Renderable::Primitive<Renderable::Quad>::instance())
{
    // Highpass program
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/highpass.frag"), Shader::Type::Fragment));
}

Hdr::~Hdr()
{
    if(fbo_ != nullptr)
        delete fbo_;
}

void Hdr::setBrightThreshold(float threshold)
{
    threshold_ = threshold;
}

void Hdr::setBlurFilter(const DownSampler::FilterPtr& filter)
{
    downSampler_.setBlurFilter(filter);
}

bool Hdr::initialize(int width, int height, int samples)
{
    if(bloomLevels_ < 1 || width < 1 || height < 1)
        return false;

    if(fbo_ != nullptr)
        delete fbo_;

    // Viewport
    width_ = width;
    height_ = height;

    // Scale fbo to half resolution
    width = std::floor(width / 2.0f);
    height = std::floor(height / 2.0f);

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

    // Calculate the smallest mipmap level
    int twidth = width;
    int theight = height;
    int tx = 0;
    int ty = 0;

    while((twidth = twidth >> 1) >= 1) ++tx;
    while((theight = theight >> 1) >= 1) ++ty;

    sampleLevel_ = qMax(tx, ty);

    if(exposureFunc_ != nullptr
        && !exposureFunc_->setInputTexture(fbo_->texture(), 1, 1, sampleLevel_))
    {
        return false;
    }

    if(tonemap_ == nullptr)
    {
        return false;
    }

    tonemap_->setInputTexture(0, samples);
    tonemap_->setBloomTexture(1, bloomLevels_);

    return downSampler_.initialise(width, height, fbo_->texture(), bloomLevels_);
}

void Hdr::render()
{
    if(inputTexture() == 0)
        return;

    else if(tonemap_ == nullptr || !highpass_.ready())
        return;

    quad_->bindVaoDirect();

    // Pass 1
    // Highpass filter
    renderHighpass();

    // Pass 2
    // Downsample and blur input
    downSampler_.downSample(fbo_->texture(), *quad_);

    // Pass 3
    // Render tonemap to output
    renderTonemap();

    gl->glBindVertexArray(0);
}

void Hdr::renderHighpass()
{
    fbo_->bind();
    highpass_->bind();

    gl->glViewport(0, 0, fbo_->width(), fbo_->height());

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(inputType(), inputTexture());

    highpass_->setUniformValue("renderedTexture", 0);
    highpass_->setUniformValue("threshold", threshold_);

    quad_->renderDirect();

    gl->glBindTexture(inputType(), 0);

    fbo_->release();

    // Generate mipmaps for exposure sampler
    if(exposureFunc_ != nullptr)
    {
        gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
        gl->glGenerateMipmap(GL_TEXTURE_2D);
        gl->glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Hdr::renderTonemap()
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, outputFbo());

    gl->glViewport(0, 0, width_, height_);

    if(!tonemap_->enable())
    {
        return;
    }

    if(exposureFunc_ != nullptr)
    {
        tonemap_->setExposure(exposureFunc_->result());
    }

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(inputType(), inputTexture());

    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());

    quad_->renderDirect();

    gl->glBindTexture(inputType(), 0);
}

void Hdr::setExposureFunction(const ExposureFuncPtr& function)
{
    exposureFunc_ = function;

    if(fbo_ != nullptr && exposureFunc_ != nullptr)
    {
        exposureFunc_->setInputTexture(fbo_->texture(), 1, 1, sampleLevel_);
    }
}

void Hdr::setHDRTonemapShader(const HDRTonemapPtr& shader)
{
    tonemap_ = shader;
}