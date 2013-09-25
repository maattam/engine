#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

#include "technique.h"

#include <QMatrix4x4>

namespace Engine {
    
class ResourceDespatcher;
    
namespace Technique {

class Skybox : public Technique
{
public:
    Skybox(ResourceDespatcher* despatcher);

    void setMVP(const QMatrix4x4& mvp);
    void setTextureUnit(unsigned int unit);

protected:
    virtual void init();

private:
    GLuint mvpLocation_;
    GLuint samplerLocation_;
};

}}

#endif // SKYBOXTECHNIQUE_H