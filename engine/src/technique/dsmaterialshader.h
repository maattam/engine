// Basic GBuffer material shader.
// The shaders loaded to this Technique should use the approriate textures for the used GBuffer.
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
    DSMaterialShader();
    virtual ~DSMaterialShader();

    // Sets the GBuffer used as input
    // precondition: gbuffer != nullptr
    // postcondition: Technique can be initialised
    void setGBuffer(GBuffer const* gbuffer);

    // Set sample count according to used MSAA level
    void setSampleCount(unsigned int count);

    // Sets the near and far clip spaces used to render the scene
    // precondition: technique is enabled
    void setDepthRange(float rnear, float rfar);

    // Sets the projection matrix used to calculate eye-space coordinate from window space.
    // precondition: technique is enabled
    void setProjMatrix(const QMatrix4x4& proj);

    void setLightDirection(const QVector3D& dir);

protected:
    virtual bool init();

private:
    GBuffer const* gbuffer_;

    int samples_;
    QVector4D viewport_;
    QVector2D depthRange_;
};

}}

#endif // DSMATERIALSHADER_H