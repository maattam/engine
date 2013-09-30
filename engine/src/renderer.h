#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include <QOpenGLShaderProgram>

#include "effect/postfx.h"
#include "graph/scenenode.h"
#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "technique/skyboxtechnique.h"
#include "renderable/quad.h"
#include "entity/entity.h"
#include "renderable/cube.h"
#include "material.h"

#include <deque>
#include <memory>

namespace Engine {

class AbstractScene;
class ResourceDespatcher;

class Renderer
{
public:
    enum RenderFlags { DEBUG_AABB = 0x1, DEBUG_WIREFRAME = 0x2 };

    explicit Renderer(ResourceDespatcher* despatcher);
    ~Renderer();

    void prepareScene(AbstractScene* scene);
    void render(AbstractScene* scene);

    bool initialize(int width, int height, int samples);

    void setFlags(unsigned int flags);
    unsigned int flags() const;

private:
    typedef std::pair<QMatrix4x4, Entity::RenderList> VisibleNode;
    typedef std::deque<VisibleNode> RenderQueue;

    std::deque<QMatrix4x4> aabbDebug_;
    Renderable::Cube aabbBox_;

    Graph::SceneNode rootNode_;

    // TODO: Move to a separate class to allow more sophisticated culling
    RenderQueue visibles_;
    std::deque<const VisibleNode*> shadowCasters_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;
    Technique::Skybox skyboxTech_;

    // For debugging depth buffer
    QOpenGLShaderProgram nullTech_;

    int width_;
    int height_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    // Postprocess chain
    std::list<Effect::Postfx*> postfxChain_;

    // Quad for postprocessing
    Renderable::Quad quad_;

    // Error material
    Material errorMaterial_;

    unsigned int flags_;

    void drawTextureDebug();

    void updateRenderQueue(Graph::SceneNode* node, const QMatrix4x4& worldView);

    void shadowMapPass(AbstractScene* scene);
    void renderPass(AbstractScene* scene, const QMatrix4x4& worldView);
    void skyboxPass(AbstractScene* scene, const QMatrix4x4& worldView);

    void destroyBuffers();

    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

}

#endif // RENDERER_H