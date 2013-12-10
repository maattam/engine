// The GBuffer holds all the textures needed for deferred rendering.

#ifndef GBUFFER_H
#define GBUFFER_H

#include "common.h"

namespace Engine {

class GBuffer
{
public:
    GBuffer();
    ~GBuffer();

    enum GBufferTexture { TEXTURE_NORMALS, TEXTURE_DIFFUSE, TEXTURE_DEPTH, TEXTURE_COUNT };

    // Allocates textures and the framebuffer object. The format is used listed below:
    // TEXTURE_NORMALS : R10G10B10A2 -> Normal.X, Normal.Y, Specular intensity, Reserved
    // TEXTURE_DIFFUSE : R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Specular power
    // TEXTURE_DEPTH   : R32F        -> Depth attachment
    bool initialise(unsigned int width, unsigned int height, unsigned int samples);

    // Binds the FBO for writing
    // MRTs: GL_COLOR_ATTACHMENT0 -> TEXTURE_NORMALS
    //       GL_COLOR_ATTACHMENT1 -> TEXTURE_DIFFUSE
    // precondition: GBuffer has been initialised
    void bindFbo();

    // Binds the textures in the order as listed in GBufferTexture.
    // The texture type is GL_TEXTURE_2D_MULTISAMPLE
    // precondition: GBuffer has been initialised and fbo has been rendered to
    void bindTextures();

private:
    GLuint fbo_;
    GLuint textures_[TEXTURE_COUNT];

    void deleteBuffers();
};

}

#endif // GBUFFER_H