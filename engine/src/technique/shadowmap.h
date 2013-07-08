#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "technique.h"

#include <QOpenGLFunctions_4_2_Core>

namespace Engine {

class ShadowMapTechnique : public Technique
{
public:
    ShadowMapTechnique(QOpenGLFunctions_4_2_Core* funcs);
    ~ShadowMapTechnique();

    bool init(unsigned int width, unsigned int height);
    virtual void enable();

    void setMVP(const QMatrix4x4& mvp);

    void bindTexture(GLenum textureUnit);

private:
    QOpenGLFunctions_4_2_Core* gl;

    GLuint fbo_;
    GLuint shadowMap_;

    void destroy();
};

}

#endif //SHADOWMAP_H