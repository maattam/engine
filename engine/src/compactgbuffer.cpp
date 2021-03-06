//
//  Author   : Matti M��tt�
//  Summary  : 
//

#include "compactgbuffer.h"

#include <QDebug>
#include <algorithm>

using namespace Engine;

CompactGBuffer::CompactGBuffer()
	: fbo_(0)
{
	std::fill(textures_, textures_ + TEXTURE_COUNT, 0);
}

CompactGBuffer::~CompactGBuffer()
{
    deleteBuffers();
}

QList<QString> CompactGBuffer::textures() const
{
	QList<QString> textures{ "normalSpec", "diffuseSpec", "depth" };
    return textures;
}

bool CompactGBuffer::initialise(unsigned int width, unsigned int height, unsigned int samples)
{
    deleteBuffers();

    // Generate textures
    gl->glGenTextures(TEXTURE_COUNT, textures_);

    // R10G10B10A2 -> Normal.X, Normal.Y, Specular data, Reserved
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textures_[TEXTURE_NORMALS]);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB10_A2, width, height, GL_TRUE);

    // R8G8B8A8 -> Diffuse.R, Diffuse.G, Diffuse.B, Specular data
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textures_[TEXTURE_DIFFUSE]);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, GL_TRUE);

    // Depth buffer, R32F
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textures_[TEXTURE_DEPTH]);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_COMPONENT32F, width, height, GL_TRUE);

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Generate and bind the framebuffer object
    gl->glGenFramebuffers(1, &fbo_);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    // Textures
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures_[TEXTURE_DEPTH], 0);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures_[TEXTURE_NORMALS], 0);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textures_[TEXTURE_DIFFUSE], 0);

    GLenum drawBuffers[TEXTURE_COUNT - 1] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    gl->glDrawBuffers(TEXTURE_COUNT - 1, drawBuffers);

    GLenum status = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        qWarning() << __FUNCTION__ << "Failed to init CompactGBuffer:" << status;
        deleteBuffers();
        return false;
    }

    return true;
}

bool CompactGBuffer::isInitialised() const
{
    return fbo_ != 0;
}

void CompactGBuffer::bindFbo()
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void CompactGBuffer::bindTextures() const
{
    for(int i = 0; i < TEXTURE_COUNT; ++i)
    {
        gl->glActiveTexture(GL_TEXTURE0 + i);
        gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textures_[i]);
    }
}

void CompactGBuffer::deleteBuffers()
{
    gl->glDeleteFramebuffers(1, &fbo_);
    gl->glDeleteTextures(TEXTURE_COUNT, textures_);

    fbo_ = 0;
	std::fill(textures_, textures_ + TEXTURE_COUNT, 0);
}