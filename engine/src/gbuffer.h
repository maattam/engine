// The GBuffer holds all the textures needed for deferred rendering.
// GBuffer is an interface for different GBuffers.

#ifndef GBUFFER_H
#define GBUFFER_H

#include <QString>
#include <vector>

namespace Engine {

class GBuffer
{
public:
    // Allocates textures and the framebuffer object. The format is used listed below:
    // postcondition: The texture type is GL_TEXTURE_2D_MULTISAMPLE
    virtual bool initialise(unsigned int width, unsigned int height, unsigned int samples) = 0;

    // Binds the FBO for writing
    // precondition: GBuffer has been initialised
    virtual void bindFbo() = 0;

    // Binds the textures in the order as listed in GBufferTexture.
    // precondition: GBuffer has been initialised and fbo has been rendered to
    // postcondition: The texture type is GL_TEXTURE_2D_MULTISAMPLE
    virtual void bindTextures() const = 0;

    // postcondition: Returns texture names ("normals", "diffuse", "depth"..) in the order of binding.
    virtual std::vector<QString> textures() const = 0;
};

}

#endif // GBUFFER_H