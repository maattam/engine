//
//  Author   : Matti Määttä
//  Summary  : GBuffer geometry shader.
//             Shaders loaded by this Technique should use the approriate textures for the used GBuffer.
//

#ifndef DSGEOMETRYSHADER_H
#define DSGEOMETRYSHADER_H

#include "technique.h"
#include "material.h"

#include <QMatrix4x4>

namespace Engine { namespace Technique {

class DSGeometryShader : public Technique
{
public:
    DSGeometryShader();
    virtual ~DSGeometryShader();

    // Set data for the current rendering stage
    // precondition: Technique has been enabled successfully.
    //               All uniforms must be set at least once before rendering.
    void setNormalMatrix(const QMatrix3x3& normalMatrix);
    void setMVP(const QMatrix4x4& mvp);
    void setMaterialAttributes(const Material::Attributes& attrib);
    void setHasTangentsAndNormals(bool value);

protected:
    virtual bool init();
};

}}

#endif // DSGEOMETRYSHADER_H