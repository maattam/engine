#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "technique.h"
#include "entity/light.h"
#include "texture2d.h"
#include "scene/visiblescene.h"

#include <QMatrix4x4>

#include <vector>

namespace Engine { namespace Technique {

class ShadowMap : public Technique
{
public:
    ShadowMap();
    virtual ~ShadowMap();

    // Initialises shadow map framebuffers and textures
    bool initDirectionalLight(GLsizei width, GLsizei height);
    bool initSpotLights(GLsizei width, GLsizei height, size_t count);
    size_t numSpotLights() const;

    // Resolves light frustrum and renders objects inside the frustrum to depth texture
    // Preconditions: Technique is enabled
    void renderDirectinalLight(Entity::Light* light, VisibleScene* visibles);
    void renderSpotLight(size_t index, const Entity::Light* light, VisibleScene* visibles);

    // Binds light depth texture to given texture unit
    bool bindDirectionalLight(GLenum textureUnit);
    bool bindSpotLight(size_t index, GLenum textureUnit);

    // Returns the light frustrum
    const QMatrix4x4& directionalLightVP() const;
    const QMatrix4x4& spotLightVP(size_t index) const;

protected:
    virtual bool init();

private:
    struct LightData
    {
        GLuint fbo;
        Texture2D::Ptr texture;
        QMatrix4x4 worldView;

        LightData();
    };

    std::vector<LightData> spotLights_;
    LightData directionalLight_;

    void renderLight(const LightData& light, VisibleScene* visibles);

    // Helper functions to help allocating and deallocating opengl objects.
    void destroySpotLights();
    void destroyLight(LightData& light);
    bool initLight(LightData& light, GLsizei width, GLsizei height);
};

}}

#endif //SHADOWMAP_H