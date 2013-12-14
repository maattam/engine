#include "hdr.h"

#include "resourcedespatcher.h"
#include "technique/blurfilter.h"
#include "samplerfunction.h"

#include <QOpenGLFramebufferObject>

using namespace Engine;
using namespace Engine::Effect;

Hdr::Hdr(ResourceDespatcher* despatcher, int bloomLevels)
    : fbo_(nullptr), downSampler_(despatcher),
    width_(0), height_(0), bloomLevels_(bloomLevels), exposure_(1.0f), sampleLevel_(0)
{
    // Tonemap program
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/tone.frag"), Shader::Type::Fragment));

    // Highpass program
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/highpass.frag"), Shader::Type::Fragment));

    // Downsampler filter
    DownSampler::FilterPtr filter = std::make_shared<Technique::BlurFilter>();
    filter->addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    filter->addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/gauss5x5.frag"), Shader::Type::Fragment));
    downSampler_.setBlurFilter(filter);
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

    return downSampler_.initialise(width, height, fbo_->texture(), bloomLevels_);
}

void Hdr::render()
{
    if(inputTexture() == 0)
        return;

    else if(!tonemap_.ready() || !highpass_.ready())
        return;

    quad_.bindVaoDirect();

    // Pass 1
    // Highpass filter
    renderHighpass();

    // Sample previous highpass result luminance
    sampleExposure();

    // Pass 2
    // Downsample and blur input
    if(!downSampler_.downSample(fbo_->texture(), quad_))
    {
        return;
    }

    // Pass 3
    // Render tonemap to output
    renderTonemap();

    gl->glBindVertexArray(0);
}

void Hdr::renderHighpass()
{
    fbo_->bind();
    gl->glClear(GL_COLOR_BUFFER_BIT);

    highpass_->bind();
    gl->glViewport(0, 0, fbo_->width(), fbo_->height());

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, inputTexture());

    highpass_->setUniformValue("renderedTexture", 0);
    highpass_->setUniformValue("threshold", 1.5f);

    quad_.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D, 0);

    fbo_->release();
}

void Hdr::renderTonemap()
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, outputFbo());
    gl->glClear(GL_COLOR_BUFFER_BIT);

    gl->glViewport(0, 0, width_, height_);

    tonemap_->bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, inputTexture());
    tonemap_->setUniformValue("renderedTexture", 0);

    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    tonemap_->setUniformValue("bloomSampler", 1);
    tonemap_->setUniformValue("bloomLevels", bloomLevels_);

    tonemap_->setUniformValue("exposure", exposure_);
    tonemap_->setUniformValue("bloomFactor", 1.0f / bloomLevels_);
    tonemap_->setUniformValue("bright", 5.0f);
    tonemap_->setUniformValue("gamma", 2.2f);

    quad_.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D, 0);
}

void Hdr::sampleExposure()
{
    if(exposureFunc_ == nullptr)
    {
        return;
    }

    // Generate mipmaps for fbo
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    exposure_ = exposureFunc_->result();
}

void Hdr::setExposureFunction(const ExposureFuncPtr& function)
{
    exposureFunc_ = function;
}