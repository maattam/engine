#ifndef BASICLIGHTNING_H
#define BASICLIGHTNING_H

#include "scene/visiblescene.h"

#include "technique.h"
#include "entity/light.h"
#include "material.h"

#include <QMatrix4x4>

#include <deque>
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

    void setDirectionalLight(Entity::Light* light);
    void setPointAndSpotLights(const VisibleScene::Lights& lights);

    void setShadowEnabled(bool value);

protected:
    virtual void init();

private:
    GLuint mvpLocation_;

    std::string formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members = "") const;

    void setSpotLight(Entity::Light* light, const QVector3D& position, int index);
    void setPointLight(Entity::Light* light, const QVector3D& position, int index);
};

}}

#endif //BASICLIGHTNING_H