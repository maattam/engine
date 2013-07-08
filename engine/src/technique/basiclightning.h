#ifndef BASICLIGHTNING_H
#define BASICLIGHTNING_H

#include "technique.h"
#include "../light.h"
#include "../material.h"

#include <QMatrix4x4>

#include <vector>
#include <string>

namespace Engine {

class BasicLightning : public Technique
{
public:
    enum { MAX_POINT_LIGHTS = 4, MAX_SPOT_LIGHTS = 4 };

    BasicLightning();
    ~BasicLightning();

    bool init();

    void setMVP(const QMatrix4x4& mvp);
    void setWorldView(const QMatrix4x4& vp);
    void setEyeWorldPos(const QVector3D& eyePos);
    void setMaterialAttributes(const Material::Attributes& attributes);
    void setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular);
    void setHasTangents(bool tangents);

    void setLightMVP(const QMatrix4x4& mvp);
    void setShadowMapTextureUnit(GLuint shadow);

    void setDirectionalLight(const DirectionalLight& light);
    void setPointLights(const std::vector<PointLight>& lights);
    void setSpotLights(const std::vector<SpotLight>& lights);

private:
    GLuint mvpLocation_;

    std::string formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members) const;
};

}

#endif //BASICLIGHTNING_H