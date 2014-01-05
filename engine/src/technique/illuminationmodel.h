#ifndef ILLUMINATIONMODEL_H
#define ILLUMINATIONMODEL_H

#include "dsmaterialshader.h"

namespace Engine {

namespace Graph {
    class Light;
}

namespace Technique {

class IlluminationModel : public DSMaterialShader
{
public:
    IlluminationModel();
    virtual ~IlluminationModel();

    // Sets the camera's view matrix used for mapping light input vectors to view space coordinates.
    void setViewMatrix(const QMatrix4x4& mat);

    // Prepares the technique for rendering spot lights.
    // Precondition: Technique is enabled, view matrix is set.
    void enableSpotLight(const Graph::Light& light);

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