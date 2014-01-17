//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "lumaexposure.h"

#include "mathelp.h"

using namespace Engine;
using namespace Engine::Effect;

LumaExposure::LumaExposure()
    : exposure_(), width_(0), height_(0), sampleLevel_(0)
{
    samplePbo_[0] = samplePbo_[1] = 0;
    writeIndex_ = 1;
    readIndex_ = 0;
}

LumaExposure::~LumaExposure()
{
    if(*samplePbo_ != 0)
    {
        gl->glDeleteBuffers(2, samplePbo_);
    }
}

bool LumaExposure::setInputTexture(GLint textureId, unsigned int width, unsigned int height, GLuint level)
{
    if(width == 0 || height == 0)
    {
        return false;
    }

    width_ = width;
    height_ = height;
    sampleLevel_ = level;

    if(*samplePbo_ != 0)
    {
        gl->glDeleteBuffers(2, samplePbo_);
    }

    int pixels = width * height * 4;

    // Create PBOs
    gl->glGenBuffers(2, samplePbo_);
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[0]);
    gl->glBufferData(GL_PIXEL_PACK_BUFFER, pixels * sizeof(float), nullptr, GL_STREAM_READ);
    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, samplePbo_[1]);
    gl->glBufferData(GL_PIXEL_PACK_BUFFER, pixels * sizeof(float), nullptr, GL_STREAM_READ);

    return true;
}

LumaExposure::ResultType LumaExposure::result()
{
    QVector3D sample = sampleTexture();
    return calculateExposure(sample);
}

QVector3D LumaExposure::sampleTexture()
{
    QVector3D result;

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
        result = QVector3D(pixel[0], pixel[1], pixel[2]);
        gl->glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    gl->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    return result;
}

float LumaExposure::calculateExposure(const QVector3D& linearSample)
{
    const float invGamma = 1.0f / 2.2f;
    const QVector3D rgbToY(0.299f, 0.587f, 0.114f);

    // RGB to YUV; Y luminance
    float Y = QVector3D::dotProduct(rgbToY, linearColor(linearSample, invGamma));
    exposure_ << Y;

    return std::exp(-2.5f * exposure_);
}