//
//  Author   : Matti Määttä
//  Summary  : Forward rendering material shader.
//

#ifndef FORWARDSHADER_H
#define FORWARDSHADER_H

#include "technique.h"
#include "material.h"

namespace Engine { namespace Technique {

class ForwardShader : public Technique
{
public:
    enum ForwardPassType { EMISSIVE_PASS, TRANSPARENT_PASS };

    ForwardShader();
    virtual ~ForwardShader();

    void setMVP(const QMatrix4x4& mvp);
    void setMaterialAttributes(const Material& material);
    void setDepthTextureUnit(int unit);

protected:
    virtual bool init();

private:
    int depthUnit_;
};

}}

#endif // FORWARDSHADER_H