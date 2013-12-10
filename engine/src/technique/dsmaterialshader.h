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

    void setViewport(const QRect& viewport);

    // Sets the near and far clip spaces used to render the scene
    // precondition: technique is enabled
    void setDepthRange(float rnear, float rfar);

    // Sets the inverse projection matrix used to calculate eye-space coordinate
    // from window space.
    // precondition: technique is enabled
    void setInvProjMatrix(const QMatrix4x4& invProj);

protected:
    virtual void init();

private:
    int samples_;
    QVector4D viewport_;

    int samplesLocation_;
    int depthRangeLocation_;
    int invProjLocation_;
    int viewportLocation_;
};

}}

#endif // DSMATERIALSHADER_H