#include "hdr.h"

#include "resourcedespatcher.h"

#include <vector>

using namespace Engine;
using namespace Engine::Effect;

Hdr::Hdr(ResourceDespatcher* despatcher, int bloomLevels)
    : fbo_(nullptr), samples_(1), downSampler_(despatcher),
    width_(0), height_(0), bloomLevels_(bloomLevels), exposure_(1.0f), sampleLevel_(0)
{
    exposures_.resize(NUM_EXPOSURES, 0.0f);

    samplePbo_[0] = samplePbo_[1] = 0;
    writeIndex_ = 1;
    readIndex_ = 0;

    // Tonemap program
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    tonemap_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/tone.frag"), Shader::Type::Fragment));

    // Highpass program
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    highpass_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/highpass.frag"), Shader::Type::Fragment));
}

Hdr::~Hdr()
{
    if(fbo_ != nullptr)
        delete fbo_;

    if(samplePbo_ != 0)
        gl->glDeleteBuffers(2, samplePbo_);
}

bool Hdr::initialize(int width, int height, int samples)
{
    if(bloomLevels_ < 1 || width < 1 || height < 1)
        return false;

    if(fbo_ != nullptr)
        delete fbo_;

    if(samplePbo_ != 0)
    {
        gl->glDeleteBuffers(2, samplePbo_);
        samplePbo_[0] = samplePbo_[1] = 0;
    }

    if(samples <= 0)
        samples = 1;

    width_ = width;
    height_ = height;

    width = std::floor(width / 2.0f);
    height = std::floor(height / 2.0f);

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

    // Create PBOs
    gl->glGenBuffers(2, samplePbo_);
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[0]);
    gl->glBufferData(GL_PIXEL_PACK_BUFFER, 4 * sizeof(float), nullptr, GL_STREAM_READ);
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[1]);
    gl->glBufferData(GL_PIXEL_PACK_BUFFER, 4 * sizeof(float), nullptr, GL_STREAM_READ);

    // Calculate the smallest mipmap level
    int twidth = width;
    int theight = height;
    int tx = 0;
    int ty = 0;

    while((twidth = twidth >> 1) >= 1) ++tx;
    while((theight = theight >> 1) >= 1) ++ty;

    sampleLevel_ = qMax(tx, ty);

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

    // Sample previous highpass result luminance
    sampleLuminance();

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
    highpass_->setUniformValue("threshold", 1.2f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    fbo_->release();
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
    tonemap_->setUniformValue("exposure", exposure_);
    tonemap_->setUniformValue("bloomFactor", 0.2f);
    tonemap_->setUniformValue("bright", 2.0f);

    quad.renderDirect();

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void Hdr::sampleLuminance()
{
    // Generate mipmaps for fbo
    gl->glBindTexture(GL_TEXTURE_2D, fbo_->texture());
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    // Swap pbos each frame
    writeIndex_ = (writeIndex_ + 1) % 2;
    readIndex_ = (readIndex_ + 1) % 2;

    // Read from gpu asynchronously
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[writeIndex_]);
    gl->glGetTexImage(GL_TEXTURE_2D, sampleLevel_, GL_RGBA, GL_FLOAT, nullptr);

    // Read previous frame's data
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[readIndex_]);
    float* pixel = static_cast<float*>(gl->glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));

    if(pixel != nullptr)
    {
        float red = pixel[0];
        float green = pixel[1];
        float blue = pixel[2];

        exposure_ = calculateExposure(red, green, blue);

        gl->glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

float Hdr::calculateExposure(float r, float g, float b)
{
    // YUV luminance
    float Y = 0.299 * r + 0.114 * g + 0.587 * b;
    Y = std::powf(Y, 1/2.2);

    //if(Y > 1.0)
    //    Y = 1.0f;

    if(exposures_.size() >= NUM_EXPOSURES)
    {
        exposures_.pop_front();
    }

    exposures_.push_back(Y);

    float average = 0.0f;
    for(float exposure : exposures_)
    {
        average += exposure;
    }

    average /= static_cast<float>(NUM_EXPOSURES);
    return std::exp(-(2.0 * average));
}