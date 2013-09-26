#ifndef HDR_H
#define HDR_H

#include "postfx.h"
#include "downsampler.h"

#include "common.h"
#include "resource/shaderprogram.h"
#include <QOpenGLFramebufferObject>

#include "renderable/quad.h"

namespace Engine {
    
    class ResourceDespatcher;

namespace Effect {

class Hdr : public Postfx
{
public:
    explicit Hdr(ResourceDespatcher* despatcher);
    ~Hdr();

    virtual bool initialize(int width, int height, int samples);
    virtual void render(const Renderable::Quad& quad);

private:
    int samples_;

    DownSampler downSampler_;
    QOpenGLFramebufferObject* fbo_;

    ShaderProgram tonemap_;
    ShaderProgram highpass_;

    void renderHighpass(const Renderable::Quad& quad);
    void renderTonemap(const Renderable::Quad& quad);
};

}}

#endif //HDR_H