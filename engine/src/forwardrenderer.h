#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "common.h"

#include "renderer.h"

#include <QRect>

#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "renderqueue.h"
#include "material.h"

namespace Engine {

namespace Graph {
    class Light;
}

class ResourceDespatcher;

class ForwardRenderer : public Renderer
{
public:
    explicit ForwardRenderer(ResourceDespatcher& despatcher);
    virtual ~ForwardRenderer();

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets lights for the current render batch.
    virtual void setLights(const QVector<LightData>& lights);

    // Sets skybox texture for the current render batch.
    virtual void setSkyboxTexture(CubemapTexture* skybox);

    // Renders the scene through the camera's viewport.
    virtual void render(Graph::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

private:
    RenderQueue* renderQueue_;
    QRect viewport_;
    unsigned int samples_;

    // Error material
    Material errorMaterial_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;

    GLuint fbo_;

    LightData directionalLight_;
    QList<LightData> lights_;

    void shadowMapPass();
    void renderPass(Graph::Camera* camera);
    void renderNode(const RenderQueue::RenderItem& node);

    bool initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples);

    ForwardRenderer(const ForwardRenderer&);
    ForwardRenderer& operator=(const ForwardRenderer&);
};

}

#endif // FORWARDRENDERER_H