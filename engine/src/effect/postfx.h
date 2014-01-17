//
//  Author   : Matti Määttä
//  Summary  : Fullscreen postprocessing effect interface.
//

#ifndef POSTFX_H
#define POSTFX_H

#include "common.h"

namespace Engine { namespace Effect {

class Postfx
{
public:
    Postfx();
    virtual ~Postfx();

    // precondition: Input/output textures must be set before calling this function.
    virtual bool initialize(int width, int height, int samples) = 0;
    virtual void render() = 0;
    
    virtual void setInputTexture(GLuint textureId, GLenum inputType = GL_TEXTURE_2D_MULTISAMPLE);
    virtual void setOutputFbo(GLuint framebufferId);

protected:
    GLuint outputFbo() const;
    GLuint inputTexture() const;
    GLenum inputType() const;

private:
    GLuint inputTexture_;
    GLuint outputFbo_;

    GLenum inputType_;

    Postfx(const Postfx&);
    Postfx& operator=(const Postfx&);
};

}}

#endif //POSTFX_H