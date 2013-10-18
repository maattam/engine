#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "texture2D.h"
#include "cubemaptexture.h"
#include "abstractscene.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "effect/hdr.h"

#include "resourcedespatcher.h"

#include <cassert>

using namespace Engine;

Renderer::Renderer(ResourceDespatcher* despatcher)
    : lightningTech_(despatcher), shadowTech_(despatcher), skyboxTech_(despatcher),
    errorMaterial_(despatcher), aabbMaterial_(despatcher), flags_(0)
{
    framebuffer_ = 0;
    renderTexture_ = 0;
    depthRenderbuffer_ = 0;
    width_ = 0;
    height_ = 0;

    // HDR postfx
    postfxChain_.push_back(new Effect::Hdr(despatcher, 4));

    // Program for debugging shadow maps
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/passthrough.vert"));
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/shadowmap.frag"));
    nullTech_.link();

    // AABB debugging tech
    aabbTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/aabb.vert"));
    aabbTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/aabb.frag"));
    aabbTech_.link();

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher->get<Texture2D>(RESOURCE_PATH("images/pink.png")));

    // AABB box
    aabbMaterial_.setAmbientColor(QVector3D(0, 0, 1));
}

Renderer::~Renderer()
{
    destroyBuffers();

    for(Effect::Postfx* fx : postfxChain_)
        delete fx;
}

void Renderer::destroyBuffers()
{
    gl->glDeleteFramebuffers(1, &framebuffer_);
    gl->glDeleteTextures(1, &renderTexture_);
    gl->glDeleteRenderbuffers(1, &depthRenderbuffer_);
}

bool Renderer::initialize(int width, int height, int samples)
{
    if(width <= 0 || height <= 0 || samples < 0)
        return false;

    destroyBuffers();
    width_ = width;
    height_ = height;

    if(!shadowTech_.initSpotLights(1024, 1024, Technique::BasicLightning::MAX_SPOT_LIGHTS))
        return false;

    if(!shadowTech_.initDirectionalLight(2048, 2048))
        return false;

    // Initialize textures
    gl->glGenTextures(1, &renderTexture_);

    // Target texture
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderTexture_);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Create depthbuffer
    gl->glGenRenderbuffers(1, &depthRenderbuffer_);
    gl->glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);

    // Initialize framebuffers
    gl->glGenFramebuffers(1, &framebuffer_);

    // Renderbuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture_, 0);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Initialize postprocess chain
    Effect::Postfx* fx = postfxChain_.front();

    if(!fx->initialize(width, height, samples))
        return false;

    fx->setInputTexture(renderTexture_);
    fx->setOutputFbo(0);    // Output to window

    return true;
}

void Renderer::render(Scene* scene, Entity::Camera* camera)
{
    aabbDebug_.clear();

    // Cull visibles
    Entity::Frustrum frustrum(camera->worldView());
    Scene::RenderQueue renderQueue;
    scene->cullVisibles(frustrum, renderQueue);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LESS);

    // Pass 1
    // Render to depth buffer
    shadowMapPass(scene);

    // Pass 2
    // Render geometry to a multisampled framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    renderPass(scene, camera, renderQueue);

    // Render skybox if set
    skyboxPass(scene, camera);

    gl->glDisable(GL_DEPTH_TEST);

    // Pass 3
    // Render postprocess chain
    for(Effect::Postfx* fx : postfxChain_)
    {
        fx->render(quad_);
    }

    //drawTextureDebug();
}

void Renderer::shadowMapPass(Scene* scene)
{
    const auto& lights = scene->queryLights();

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_FRONT);

    if(!shadowTech_.enable())
    {
        return;
    }

    Scene::RenderQueue shadowCasters;
    int spotLightIndex = 0;

    // Render depth for each spot light
    for(const Entity::VisibleLight& light : lights)
    {
        // Ignore other lights for now
        if(light.second->type() != Entity::Light::LIGHT_SPOT)
        {
            continue;
        }

        shadowTech_.enableSpotLight(spotLightIndex, light);
        gl->glClear(GL_DEPTH_BUFFER_BIT);

        // Resolve visibles for each light frustrum
        const QMatrix4x4& spotVP = shadowTech_.spotLightVP(spotLightIndex);
        Entity::Frustrum frustrum(spotVP);
        scene->cullVisibles(frustrum, shadowCasters, true);

        for(auto it = shadowCasters.begin(); it != shadowCasters.end(); ++it)
        {
            const RenderList& node = it->second;
            shadowTech_.setLightMVP(spotVP * it->first);

            for(auto rit = node.begin(); rit != node.end(); ++rit)
            {
                rit->second->render();
            }
        }

        spotLightIndex++;
    }

    // Render depth for directional light
    auto directionalLight = scene->directionalLight();
    if(directionalLight == nullptr)
    {
        return;
    }

    shadowTech_.enableDirectinalLight(directionalLight);
    gl->glClear(GL_DEPTH_BUFFER_BIT);

    // Resolve occluders for directional light
    const QMatrix4x4& dirVP = shadowTech_.directionalLightVP();
    Entity::Frustrum frustrum(dirVP);
    scene->cullVisibles(frustrum, shadowCasters, true);

    for(auto it = shadowCasters.begin(); it != shadowCasters.end(); ++it)
    {
        const RenderList& node = it->second;
        shadowTech_.setLightMVP(dirVP * it->first);

        for(auto rit = node.begin(); rit != node.end(); ++rit)
        {
            rit->second->render();
        }
    }
}

void Renderer::renderPass(Scene* scene, Entity::Camera* camera, const Scene::RenderQueue& visibles)
{
    const auto& lights = scene->queryLights();

    // Prepare OpenGL state for render pass
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    gl->glCullFace(GL_BACK);
    gl->glViewport(0, 0, width_, height_);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(camera->position());
    lightningTech_.setTextureUnits(0, 1, 2);
    lightningTech_.setDirectionalLight(scene->directionalLight());
    lightningTech_.setPointAndSpotLights(scene->queryLights());

    // Bind directional light shadow map
    shadowTech_.bindDirectionalLight(GL_TEXTURE0 + Material::TEXTURE_COUNT);
    lightningTech_.setDirectionalLightShadowUnit(Material::TEXTURE_COUNT);

    // Bind spot light shadow maps after material samplers
    int spotLightIndex = 0;
    for(const Entity::VisibleLight& light : lights)
    {
        if(light.second->type() == Entity::Light::LIGHT_SPOT)
        {
            int index = Material::TEXTURE_COUNT + spotLightIndex + 1;

            shadowTech_.bindSpotLight(spotLightIndex, GL_TEXTURE0 + index);
            lightningTech_.setSpotLightShadowUnit(spotLightIndex, index);

            spotLightIndex++;
        }
    }

    // Set polygonmode if wireframe mode is enabled
    if(flags_ & DEBUG_WIREFRAME)
    {
        gl->glDisable(GL_CULL_FACE);
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        Material::Attributes attrib;
        attrib.ambientColor = QVector3D(0.3f, 0.3f, 0.3f);  // Hilight polygons slightly

        lightningTech_.setMaterialAttributes(attrib);
    }

    // Render visibles
    for(auto it = visibles.begin(); it != visibles.end(); ++it)
    {
        lightningTech_.setWorldView(it->first);
        lightningTech_.setMVP(camera->worldView() * it->first);

        // Set translation matrix for each spot light
        spotLightIndex = 0;
        for(const Entity::VisibleLight& light : lights)
        {
            if(light.second->type() == Entity::Light::LIGHT_SPOT)
            {
                const QMatrix4x4& spotVP = shadowTech_.spotLightVP(spotLightIndex);
                lightningTech_.setSpotLightMVP(spotLightIndex, spotVP * it->first);
                spotLightIndex++;
            }
        }

        // Set directional light mvp
        lightningTech_.setDirectionalLightMVP(shadowTech_.directionalLightVP() * it->first);

        renderNode(it->second);
    }

    // Reset polygonmode
    if(flags_ & DEBUG_WIREFRAME)
    {
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gl->glEnable(GL_CULL_FACE);
    }

    // Draw bounding boxes
    if(flags_ & DEBUG_AABB)
    {
        aabbTech_.bind();
        aabbTech_.setUniformValue("gColor", aabbMaterial_.attributes().ambientColor);

        gl->glDisable(GL_CULL_FACE);
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for(auto it = aabbDebug_.begin(); it != aabbDebug_.end(); ++it)
        {
            aabbTech_.setUniformValue("gMVP", camera->worldView() * (*it));
            aabbBox_.render();
        }

        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gl->glEnable(GL_CULL_FACE);
    }
}

void Renderer::renderNode(const RenderList& node)
{
    for(auto it = node.begin(); it != node.end(); ++it)
    {
        Material* material = (*it).first;
        Renderable::Renderable* renderable = (*it).second;

        if(!material->bind())
        {
            material = &errorMaterial_;     // Draw error material to indicate a problem with the object

            if(!errorMaterial_.bind())      // We can't fail further
                continue;
        }

        lightningTech_.setHasTangents(renderable->hasTangents() && material->hasNormals());

        // Ignore material attributes in wireframe mode
        if(!(flags_ & DEBUG_WIREFRAME))
        {
            lightningTech_.setMaterialAttributes(material->attributes());
        }

        renderable->render();
    }
}

void Renderer::skyboxPass(Scene* scene, Entity::Camera* camera)
{
    if(flags_ & DEBUG_WIREFRAME)    // Don't draw skybox in wireframe mode to help debugging
        return;

    if(scene->skybox() == nullptr)
        return;

    if(!skyboxTech_.enable())
        return;

    QMatrix4x4 trans;
    trans.translate(camera->position());

    // Move cube so it covers our entire view
    skyboxTech_.setMVP(camera->worldView() * trans);
    skyboxTech_.setTextureUnit(0);

    if(scene->skybox()->bindActive(GL_TEXTURE0))
    {
        // We want to see the skybox texture from the inside
        gl->glCullFace(GL_FRONT);
        gl->glDepthFunc(GL_LEQUAL);

        aabbBox_.render();

        gl->glCullFace(GL_BACK);
    }
}

void Renderer::drawTextureDebug()
{
    shadowTech_.bindDirectionalLight(GL_TEXTURE0);
    nullTech_.bind();
    nullTech_.setUniformValue("gShadowMap", 0);

    gl->glViewport(0, 0, 512, 512);

    quad_.render();

    gl->glViewport(0, 0, width_, height_);
}

void Renderer::setFlags(unsigned int flags)
{
    flags_ = flags;
}

unsigned int Renderer::flags() const
{
    return flags_;
}

void Renderer::addAABBDebug(const QMatrix4x4& trans, const Entity::AABB& aabb)
{
    if(aabb.width() <= 0)
        return;

    QMatrix4x4 scale;
    scale.translate(aabb.center());
    scale.scale(0.5f);  // Our bounding box model is 2 units wide
    scale.scale(aabb.width(), aabb.height(), aabb.depth());   // Display over mesh surface

    aabbDebug_.push_back(trans * scale);
}