#include "postfx.h"

using namespace Engine;
using namespace Engine::Effect;

Postfx::Postfx() 
    : inputTexture_(0), outputFbo_(0), inputType_(GL_TEXTURE_2D_MULTISAMPLE)
{
}

Postfx::~Postfx()
{
}
    
void Postfx::setInputTexture(GLuint textureId, GLenum inputType)
{
    inputTexture_ = textureId;
    inputType = inputType;
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

GLenum Postfx::inputType() const
{
    return inputType_;
}