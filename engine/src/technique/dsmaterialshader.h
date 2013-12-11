#ifndef DSMATERIALSHADER_H
#define DSMATERIALSHADER_H

#include "technique.h"

#include <QMatrix4x4>
#include <QRect>

namespace Engine { namespace Technique {

class DSMaterialShader : public Technique
{
public:
    DSMaterialShader();
    virtual ~DSMaterialShader();

    // Set sample count according to use MSAA level
    void setSampleCount(unsigned int count);

    // Sets the near and far clip spaces used to render the scene
    // precondition: technique is enabled
    void setDepthRange(float rnear, float rfar);

    // Sets the projection matrix used to calculate eye-space coordinate from window space.
    // precondition: technique is enabled
    void setProjMatrix(const QMatrix4x4& proj);

protected:
    virtual void init();

private:
    int samples_;
    QVector4D viewport_;
    QVector2D depthRange_;
};

}}

#endif // DSMATERIALSHADER_H