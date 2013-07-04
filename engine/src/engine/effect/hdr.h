#ifndef HDR_H
#define HDR_H

#include "postfx.h"
#include "downsampler.h"

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

namespace Engine {

class Hdr : public Postfx
{
public:
    Hdr(QOpenGLFunctions_4_2_Core* funcs);
    ~Hdr();

    virtual bool initialize(int width, int height);
    virtual void render(GLuint vao, GLsizei size);

private:
    QOpenGLFunctions_4_2_Core* gl;

    DownSampler downSampler_;
    QOpenGLFramebufferObject* fbo_;

    QOpenGLShaderProgram tonemap_;
    QOpenGLShaderProgram highpass_;
    QOpenGLShaderProgram null_;

    void renderHighpass(GLsizei size);
    void blendSamples(GLsizei size);
    void renderTonemap(GLsizei size);
};

}

#endif //HDR_H