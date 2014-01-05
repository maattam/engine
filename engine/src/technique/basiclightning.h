#ifndef BASICLIGHTNING_H
#define BASICLIGHTNING_H

#include "technique.h"
#include "material.h"

#include <QMatrix4x4>

#include <QList>
#include <string>

namespace Engine { 

namespace Graph {
    class Light;
}

namespace Technique {

class BasicLightning : public Technique
{
public:
    enum { MAX_POINT_LIGHTS = 30, MAX_SPOT_LIGHTS = 4 };

    BasicLightning();
    virtual ~BasicLightning();

    void setMVP(const QMatrix4x4& mvp);
    void setWorldView(const QMatrix4x4& vp);
    void setEyeWorldPos(const QVector3D& eyePos);
    void setMaterialAttributes(const Material::Attributes& attributes);
    void setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular, GLuint mask);
    void setHasTangents(bool tangents);

    void setSpotLightMVP(size_t index, const QMatrix4x4& mvp);
    void setSpotLightShadowUnit(size_t index, GLuint shadow);

    void setDirectionalLightMVP(const QMatrix4x4& mvp);
    void setDirectionalLightShadowUnit(GLuint shadow);

    void setDirectionalLight(Graph::Light* light);
    void setPointAndSpotLights(const QList<Graph::Light*>& lights);

    void setShadowEnabled(bool value);

protected:
    virtual bool init();

private:
    std::string formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members = "") const;

    void setSpotLight(const Graph::Light* light, int index);
    void setPointLight(const Graph::Light* light, int index);
};

}}

#endif //BASICLIGHTNING_H