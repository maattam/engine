#include "postfx.h"

using namespace Engine;

Postfx::Postfx()
    : inputTexture_(0), outputFbo_(0)
{
}

Postfx::~Postfx()
{
}
    
void Postfx::setInputTexture(GLuint textureId)
{
    inputTexture_ = textureId;
}

void Postfx::setOutputFbo(GLuint framebufferId)
{
    outputFbo_ = framebufferId;
}

GLuint Postfx::outputFbo() const
{
    return outputFbo_;
}

GLuint Postfx::inputTexture() const
{
    return inputTexture_;
}