#ifndef BASICLIGHTNING_H
#define BASICLIGHTNING_H

#include "technique.h"
#include "entity/light.h"
#include "material.h"

#include <QMatrix4x4>

#include <vector>
#include <string>

namespace Engine {
    
class ResourceDespatcher;

namespace Technique {

class BasicLightning : public Technique
{
public:
    enum { MAX_POINT_LIGHTS = 4, MAX_SPOT_LIGHTS = 4 };

    BasicLightning(ResourceDespatcher* despatcher);
    ~BasicLightning();

    void setMVP(const QMatrix4x4& mvp);
    void setWorldView(const QMatrix4x4& vp);
    void setEyeWorldPos(const QVector3D& eyePos);
    void setMaterialAttributes(const Material::Attributes& attributes);
    void setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular);
    void setHasTangents(bool tangents);

    void setSpotLightMVP(size_t index, const QMatrix4x4& mvp);
    void setSpotLightShadowUnit(size_t index, GLuint shadow);

    void setDirectionalLightMVP(const QMatrix4x4& mvp);
    void setDirectionalLightShadowUnit(GLuint shadow);

    void setDirectionalLight(const Entity::DirectionalLight& light);
    void setPointLights(const std::vector<Entity::PointLight>& lights);
    void setSpotLights(const std::vector<Entity::SpotLight>& lights);

protected:
    virtual void init();

private:
    GLuint mvpLocation_;

    std::string formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members = "") const;
};

}}

#endif //BASICLIGHTNING_H