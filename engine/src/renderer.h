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

#include "scene.h"

#include <deque>
#include <memory>

namespace Engine {

class ResourceDespatcher;

namespace Entity {
    class Camera;
}

class Renderer
{
public:
    enum RenderFlags { DEBUG_AABB = 0x1, DEBUG_WIREFRAME = 0x2 };

    explicit Renderer(ResourceDespatcher* despatcher);
    ~Renderer();

    void render(Scene* scene, Entity::Camera* camera);

    bool initialize(int width, int height, int samples);

    void setFlags(unsigned int flags);
    unsigned int flags() const;

private:
    std::deque<QMatrix4x4> aabbDebug_;
    Renderable::Cube aabbBox_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;
    Technique::Skybox skyboxTech_;

    // For debugging depth buffer
    QOpenGLShaderProgram nullTech_;

    // For rendering aabb's efficiently
    QOpenGLShaderProgram aabbTech_;

    int width_;
    int height_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    // Postprocess chain
    std::list<Effect::Postfx*> postfxChain_;

    // Quad for postprocessing
    Renderable::Quad quad_;

    // Error and aabb material
    Material errorMaterial_;
    Material aabbMaterial_;

    unsigned int flags_;

    void drawTextureDebug();

    void shadowMapPass(Scene* scene);
    void renderPass(Scene* scene, Entity::Camera* camera, const Scene::RenderQueue& queue, const QMatrix4x4& worldView);
    void skyboxPass(Scene* scene, Entity::Camera* camera, const QMatrix4x4& worldView);

    void renderNode(const RenderList& node);

    void destroyBuffers();
    void addAABBDebug(const QMatrix4x4& trans, const Entity::AABB& aabb);

    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

}

#endif // RENDERER_H