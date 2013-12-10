#ifndef DSGEOMETRYSHADER_H
#define DSGEOMETRYSHADER_H

#include "technique.h"
#include "material.h"

#include <QMatrix4x4>

namespace Engine { namespace Technique {

class DSGeometryShader : public Technique
{
public:
    DSGeometryShader(ResourceDespatcher* despatcher);
    virtual ~DSGeometryShader();

    // Set data for the current rendering stage
    // precondition: Technique has been enabled successfully.
    //               All uniforms must be set at least once before rendering.
    void setModelViewMatrix(const QMatrix4x4& modelView);
    void setMVP(const QMatrix4x4& mvp);
    void setMaterialAttributes(const Material::Attributes& attrib);
    void setHasTangentsAndNormals(bool value);

protected:
    virtual void init();

private:
    int modelViewLocation_;
    int mvpLocation_;

    struct MaterialLocation
    {
        int diffuseColor;
        int shininess;
        int specularIntensity;
    };

    MaterialLocation materialLocation_;

    GLuint skipTangentIndex_;
    GLuint calcTangentIndex_;

    GLuint calcNormalIndex_;
    GLuint interpNormalIndex_;
};

}}

#endif // DSGEOMETRYSHADER_H