#ifndef HDR_H
#define HDR_H

#include "postfx.h"

#include "common.h"
#include "downsampler.h"
#include "shaderprogram.h"
#include "renderable/quad.h"

#include <memory>

class QOpenGLFramebufferObject;

namespace Engine {
    
class ResourceDespatcher;

namespace Technique
{
    class HDRTonemap;
}

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
    typedef std::shared_ptr<Technique::HDRTonemap> HDRTonemapPtr;
    typedef DownSampler::FilterPtr BlurFilterPtr;

    // Sets the exposure function for automatic adaptation.
    // If the function is null, no automatic correction is used.
    // postcondition: Initialize must be called after calling this function.
    void setExposureFunction(const ExposureFuncPtr& function);

    // Sets the HDR tonemap shader technique
    // precondition: shader != nullptr
    // postcondition: Shader ownership is maintained,
    //                initialize must be called after calling this function.
    void setHDRTonemapShader(const HDRTonemapPtr& shader);

    // Sets the blur method for calculating bloom.
    // precondition: filter != nullptr
    // postcondition: Filter ownership is shared
    void setBlurFilter(const BlurFilterPtr& filter);

    // Sets the highpass filter threshold when calculating bloom.
    void setBrightThreshold(float threshold);

private:
    int width_;
    int height_;
    int bloomLevels_;
    int sampleLevel_;
    float threshold_;

    DownSampler downSampler_;
    Renderable::Quad quad_;
    QOpenGLFramebufferObject* fbo_;

    ShaderProgram highpass_;

    ExposureFuncPtr exposureFunc_;
    HDRTonemapPtr tonemap_;

    void renderHighpass();
    void renderTonemap();
};

}}

#endif //HDR_H