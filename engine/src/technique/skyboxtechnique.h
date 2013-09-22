#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

#include "technique.h"

#include <QMatrix4x4>

namespace Engine { namespace Technique {

class Skybox : public Technique
{
public:
    Skybox();

    void setMVP(const QMatrix4x4& mvp);
    void setTextureUnit(unsigned int unit);

private:
    GLuint mvpLocation_;
    GLuint samplerLocation_;
};

}}

#endif // SKYBOXTECHNIQUE_H