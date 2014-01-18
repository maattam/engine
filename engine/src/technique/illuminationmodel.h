//
//  Author   : Matti Määttä
//  Summary  : Deferred shading illumination model for accumulating spot, point and
//             directional lights.
//

#ifndef ILLUMINATIONMODEL_H
#define ILLUMINATIONMODEL_H

#include "dsmaterialshader.h"

namespace Engine {

namespace Graph {
    class Light;
    class Camera;
}

class ShadowMap;

namespace Technique {

class IlluminationModel : public DSMaterialShader
{
public:
    explicit IlluminationModel(unsigned int samples);
    virtual ~IlluminationModel();

    void setCamera(const Graph::Camera& camera);

    void setQuadExtents(float scale, const QVector3D& center);

    // Prepares the technique for rendering spot lights.
    // Precondition: Technique is enabled, view matrix is set.
    void enableSpotLight(const Graph::Light& light, ShadowMap* shadow = nullptr);

    // Prepares the technique for rendering point lights.
    // Precondition: Technique is enabled, view matrix is set.
    void enablePointLight(const Graph::Light& light);

    // Prepares the technique for rendering directional lights.
    // Precondition: Technique is enabled, view matrix is set.
    void enableDirectionalLight(const Graph::Light& light);

protected:
    virtual bool init();

private:
    QMatrix4x4 view_;
    QString lightningModel_;

    void setPointUniforms(const Graph::Light& spot);
};

}}

#endif // ILLUMINATIONMODEL_H