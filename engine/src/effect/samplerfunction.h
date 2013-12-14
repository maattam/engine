// Interface class for different texture samplers

#ifndef SAMPLERFUNCTION_H
#define SAMPLERFUNCTION_H

#include "common.h"

namespace Engine { namespace Effect {

template<typename Result>
class SamplerFunction
{
public:
    virtual ~SamplerFunction() {};

    typedef Result ResultType;

    // Sets the input texture.
    // precondition: textureId is valid, width and height correspond to the mipmap level.
    virtual bool setInputTexture(GLint textureId, unsigned int width, unsigned int height, GLuint level) = 0;

    // Returns the result of the function.
    // precondition: texture has been set.
    virtual ResultType result() = 0;
};

}};

#endif // SAMPLERFUNCTION_H