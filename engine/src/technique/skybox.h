#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

#include "technique.h"

#include <QMatrix4x4>

namespace Engine {
    
namespace Technique {

class Skybox : public Technique
{
public:
    Skybox();
    virtual ~Skybox();

    // Sets the texture location of the bound CubemapTexture
    void setTextureUnit(int unit);

    // Set the sample count for averaging depth when using deferred rendering.
    void setSampleCount(int samples);

    // Sets the MVP matrix
    // precondition: Technique is enabled
    void setMVP(const QMatrix4x4& mvp);

    // Sets the brightness multiplier. Default is 1.0f.
    // precondition: brightness >= 0
    void setBrightness(float brightness);

    // Sets the texture unit for sampling depth instead of the bound framebuffer
    void setDepthTextureUnit(int unit);

protected:
    virtual bool init();

private:
    int textureUnit_;
    int depthUnit_;
    int samples_;
    float brightness_;
};

}}

#endif // SKYBOXTECHNIQUE_H