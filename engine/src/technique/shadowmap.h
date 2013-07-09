#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "technique.h"
#include "light.h"

#include <QOpenGLFunctions_4_2_Core>
#include <QMatrix4x4>

#include <vector>

namespace Engine {

class ShadowMapTechnique : public Technique
{
public:
    ShadowMapTechnique(QOpenGLFunctions_4_2_Core* funcs);
    ~ShadowMapTechnique();

    bool initSpotLights(unsigned int width, unsigned int height, size_t count);
    void enableSpotLight(size_t index, const SpotLight& spotLight);
    void bindSpotLight(size_t index, GLenum textureUnit);
    const QMatrix4x4& spotLightVP(size_t index) const;

    void setLightMVP(const QMatrix4x4& mvp);

private:
    QOpenGLFunctions_4_2_Core* gl;

    std::vector<GLuint> spotLightFbos_;
    std::vector<GLuint> spotLightTextures_;
    std::vector<QMatrix4x4> spotLightVPs_;

    unsigned int spotWidth_;
    unsigned int spotHeight_;

    void destroySpotLights();
};

}

#endif //SHADOWMAP_H