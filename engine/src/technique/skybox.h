//
//  Author   : Matti Määttä
//  Summary  : Skybox shader for both deferred and forward pass.
//

#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

#include "technique.h"

#include <QMatrix4x4>

namespace Engine {
    
namespace Technique {

class Skybox : public Technique
{
public:
    explicit Skybox(unsigned int samples);
    virtual ~Skybox();

    // Sets the texture location of the bound CubemapTexture
    void setTextureUnit(int unit);

    // Sets the MVP matrix
    // precondition: Technique is enabled
    void setMVP(const QMatrix4x4& mvp);

    // Adds a new shader to the program
    // precondition: shader not null
    // postcondition: shader will be linked when enable is called
    virtual void addShader(const Shader::Ptr& shader);

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

    unsigned int samples_;
    float brightness_;
};

}}

#endif // SKYBOXTECHNIQUE_H