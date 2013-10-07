#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "technique.h"
#include "entity/light.h"
#include "texture2d.h"

#include <QMatrix4x4>

#include <vector>

namespace Engine {
    
class ResourceDespatcher;

namespace Technique {

class ShadowMap : public Technique
{
public:
    ShadowMap(ResourceDespatcher* despatcher);
    ~ShadowMap();

    bool initDirectionalLight(GLsizei width, GLsizei height);
    void enableDirectinalLight(const Entity::DirectionalLight& light);
    void bindDirectionalLight(GLenum textureUnit);
    const QMatrix4x4& directionalLightVP() const;

    bool initSpotLights(GLsizei width, GLsizei height, size_t count);
    void enableSpotLight(size_t index, const Entity::SpotLight& spotLight);
    void bindSpotLight(size_t index, GLenum textureUnit);
    const QMatrix4x4& spotLightVP(size_t index) const;

    void setLightMVP(const QMatrix4x4& mvp);

private:
    std::vector<GLuint> spotLightFbos_;
    std::vector<Texture2D::Ptr> spotLightTextures_;
    std::vector<QMatrix4x4> spotLightVPs_;

    GLuint directionalLightFbo_;
    Texture2D directionalLightTexture_;
    QMatrix4x4 directionalLightVP_;

    void destroySpotLights();
    void destroyDirectionalLight();

    bool initDepthFBO(GLuint fbo, Texture2D& texture, GLsizei width, GLsizei height);
};

}}

#endif //SHADOWMAP_H