// Basic GBuffer material shader.
// Shaders loaded by this Technique should use the approriate textures for the used GBuffer.
// The shader's uniforms should contain the associated samplers in the format textureName + Data, f.e. "normalData" for normals.

#ifndef DSMATERIALSHADER_H
#define DSMATERIALSHADER_H

#include "technique.h"

#include <QMatrix4x4>
#include <QRect>

namespace Engine {

class GBuffer;

namespace Technique {

class DSMaterialShader : public Technique
{
public:
    explicit DSMaterialShader(unsigned int samples);
    virtual ~DSMaterialShader();

    // Sets the GBuffer used as input
    // precondition: gbuffer != nullptr
    // postcondition: Technique can be initialised
    void setGBuffer(GBuffer const* gbuffer);

    // Adds a new shader to the program
    // precondition: shader not null
    // postcondition: shader will be linked when enable is called
    virtual void addShader(const Shader::Ptr& shader);

    // Sets the projection matrix used to calculate eye-space coordinate from window space.
    // precondition: technique is enabled
    void setProjMatrix(const QMatrix4x4& proj);

    void setViewport(const QRect& viewport);

protected:
    virtual bool init();

private:
    GBuffer const* gbuffer_;

    QVector4D viewport_;
    unsigned int samples_;
};

}}

#endif // DSMATERIALSHADER_H