#ifndef HDR_H
#define HDR_H

#include "postfx.h"
#include "downsampler.h"

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

#include "renderable/quad.h"

namespace Engine {

class Hdr : public Postfx
{
public:
    Hdr(QOpenGLFunctions_4_2_Core* funcs);
    ~Hdr();

    virtual bool initialize(int width, int height, int samples);
    virtual void render(const Quad& quad);

private:
    QOpenGLFunctions_4_2_Core* gl;

    int samples_;

    DownSampler downSampler_;
    QOpenGLFramebufferObject* fbo_;

    QOpenGLShaderProgram tonemap_;
    QOpenGLShaderProgram highpass_;
    QOpenGLShaderProgram null_;

    void renderHighpass(const Quad& quad);
    void blendSamples(const Quad& quad);
    void renderTonemap(const Quad& quad);
};

}

#endif //HDR_H