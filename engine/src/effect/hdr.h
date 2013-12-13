#ifndef HDR_H
#define HDR_H

#include "postfx.h"
#include "downsampler.h"

#include "common.h"
#include "shaderprogram.h"
#include <QOpenGLFramebufferObject>

#include "renderable/quad.h"

#include <list>

namespace Engine {
    
    class ResourceDespatcher;

namespace Effect {

class Hdr : public Postfx
{
public:
    Hdr(ResourceDespatcher* despatcher, int bloomLevels);
    ~Hdr();

    virtual bool initialize(int width, int height, int samples);
    virtual void render();

private:
    int width_;
    int height_;
    int bloomLevels_;

    float exposure_;

    DownSampler downSampler_;
    Renderable::Quad quad_;
    QOpenGLFramebufferObject* fbo_;

    ShaderProgram tonemap_;
    ShaderProgram highpass_;

    GLuint samplePbo_[2];
    GLuint sampleLevel_;
    int writeIndex_;
    int readIndex_;

    void renderHighpass();
    void renderTonemap();

    // Samples the last frame's luminance and calculates new exposure
    void sampleLuminance();
    float calculateExposure(float r, float g, float b);

    enum { NUM_EXPOSURES = 30 };

    std::list<float> exposures_;
};

}}

#endif //HDR_H