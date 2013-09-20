#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "technique.h"
#include "entity/light.h"

#include <QMatrix4x4>

#include <vector>

namespace Engine { namespace Technique {

class ShadowMap : public Technique
{
public:
    ShadowMap();
    ~ShadowMap();

    bool initSpotLights(unsigned int width, unsigned int height, size_t count);
    void enableSpotLight(size_t index, const Entity::SpotLight& spotLight);
    void bindSpotLight(size_t index, GLenum textureUnit);
    const QMatrix4x4& spotLightVP(size_t index) const;

    void setLightMVP(const QMatrix4x4& mvp);

private:
    std::vector<GLuint> spotLightFbos_;
    std::vector<GLuint> spotLightTextures_;
    std::vector<QMatrix4x4> spotLightVPs_;

    unsigned int spotWidth_;
    unsigned int spotHeight_;

    void destroySpotLights();
};

}}

#endif //SHADOWMAP_H