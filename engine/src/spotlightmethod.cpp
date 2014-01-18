//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "spotlightmethod.h"

#include "singleshadowmap.h"
#include "scene/sceneobservable.h"
#include "graph/light.h"
#include "graph/camera.h"
#include "graph/scenenode.h"
#include "graph/sceneleaf.h"
#include "mathelp.h"
#include "resourcedespatcher.h"

using namespace Engine;

SpotLightMethod::SpotLightMethod(ResourceDespatcher& despatcher)
    : shadow_(nullptr), scene_(nullptr), initTech_(false), cachedVP_(nullptr)
{
    // Load shaders
    tech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/shadowmap.vert"), Shader::Type::Vertex));
    tech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/shadowmap.frag"), Shader::Type::Fragment));

    // Set technique and OnRenderCallback for renderer.
    renderer_.setRenderCallback([this] (Material& mat, const QMatrix4x4& mvp)
        {
            if(!initTech_)
            {
                tech_.setUniformValue("maskSampler", Material::TEXTURE_MASK);
                initTech_ = true;
            }

            // Bind mask texture
            mat.getTexture(Material::TEXTURE_MASK)->bind(GL_TEXTURE0 + Material::TEXTURE_MASK);
            tech_.setUniformValue("MVP", *cachedVP_ * mvp);
        }
    );

    renderer_.setTechnique(&tech_);
}

SpotLightMethod::~SpotLightMethod()
{
}

ShadowMap* SpotLightMethod::createShadowMap()
{
    return new SingleShadowMap();
}

void SpotLightMethod::setShadowMap(ShadowMap* shadow)
{
#ifdef _DEBUG
    Q_ASSERT(dynamic_cast<SingleShadowMap*>(shadow));
#endif

    shadow_ = static_cast<SingleShadowMap*>(shadow);
}

void SpotLightMethod::setCamera(Graph::Camera* /*camera*/)
{
}

void SpotLightMethod::setSceneObservable(SceneObservable* observable)
{
    scene_ = observable;
}

void SpotLightMethod::prepare(Graph::Light& light)
{
    // Reset batch
    shadow_->batch().clear();

    // Set up frustum that fills the light's field of view.
    // TODO: Detach Graph::Camera from frustrum implementation so we can pass that to renderer
    // instead of using callback to calculate MVP
    QMatrix4x4 lightView;
    lightView.perspective(light.angleOuterCone() * 2, 1.0f, 1.0f, light.cutoffDistance());
    lightView.lookAt(light.position(), light.position() + light.direction(), UNIT_Y);

    shadow_->setLightVP(lightView);

    // Query renderables inside the camera
    RenderQueue& visibles = shadow_->batch();
    unsigned int lightMask = light.lightMask();

    scene_->findVisibleLeaves(lightView, visibles,
        [lightMask] (const Graph::SceneLeaf&, const Graph::SceneNode& node)
        {
            // Accept only renderables that cast shadows by this light.
            return (lightMask & node.lightMask()) == lightMask;
        }
    );
}

void SpotLightMethod::render()
{
    renderer_.setGeometryBatch(&shadow_->batch());
    renderer_.setRenderTarget(shadow_->fboHandle());
    renderer_.setViewport(QRect(QPoint(0, 0), shadow_->size()), 1);

    cachedVP_ = &shadow_->lightVP();

    if(shadow_->bindFbo())
    {
        // Cull front faces to reduce self-shadowing
        gl->glCullFace(GL_FRONT);

        renderer_.render();

        gl->glCullFace(GL_BACK);
    }

    cachedVP_ = nullptr;

    // Reset batch
    shadow_->batch().clear();
}