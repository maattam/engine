#ifndef HDR_H
#define HDR_H

#include "postfx.h"

#include "common.h"
#include "downsampler.h"
#include "shaderprogram.h"
#include "renderable/quad.h"

#include <list>
#include <memory>

class QOpenGLFramebufferObject;

namespace Engine {
    
class ResourceDespatcher;

namespace Effect {

template<typename T>
class SamplerFunction;

class Hdr : public Postfx
{
public:
    Hdr(ResourceDespatcher* despatcher, int bloomLevels);
    ~Hdr();

    virtual bool initialize(int width, int height, int samples);
    virtual void render();

    typedef std::shared_ptr<SamplerFunction<float>> ExposureFuncPtr;

    // Sets the exposure function for automatic adaptation.
    // If the function is null, no automatic correction is used.
    // postcondition: Initialize must be called after calling this function.
    void setExposureFunction(const ExposureFuncPtr& function);

private:
    int width_;
    int height_;
    int bloomLevels_;
    int sampleLevel_;

    float exposure_;

    DownSampler downSampler_;
    Renderable::Quad quad_;
    QOpenGLFramebufferObject* fbo_;

    ShaderProgram tonemap_;
    ShaderProgram highpass_;

    ExposureFuncPtr exposureFunc_;

    void renderHighpass();
    void renderTonemap();
    void sampleExposure();
};

}}

#endif //HDR_H