#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "texture.h"
#include "renderable.h"
#include "abstractscene.h"
#include "camera.h"
#include "light.h"
#include "effect/hdr.h"

using namespace Engine;

const GLfloat QUAD_VERTEX_DATA[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

Renderer::Renderer(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), quadVao_(0), quadVertexBuffer_(0), framebuffer_(0), renderTexture_(0), depthRenderbuffer_(0),
    width_(0), height_(0)
{
    // Buffer quad
    gl->glGenVertexArrays(1, &quadVao_);
    gl->glBindVertexArray(quadVao_);

    gl->glGenBuffers(1, &quadVertexBuffer_);
    gl->glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer_);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTEX_DATA), QUAD_VERTEX_DATA, GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glBindVertexArray(0);

    // HDR postfx
    postfxChain_.push_back(new Hdr(funcs));
}

Renderer::~Renderer()
{
    destroyBuffers();
    gl->glDeleteVertexArrays(1, &quadVao_);

    for(Postfx* fx : postfxChain_)
        delete fx;
}

void Renderer::prepareScene(AbstractScene* scene)
{
    rootNode_.removeAllChildren();

    if(scene != nullptr)
    {
        scene->prepareScene(&rootNode_);
    }
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

    if(!technique_.init())
        return false;

    destroyBuffers();
    width_ = width;
    height_ = height;

    // Initialize textures
   gl->glGenTextures(1, &renderTexture_);

    // Target texture
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderTexture_);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Create depthbuffer
    gl->glGenRenderbuffers(1, &depthRenderbuffer_);
    gl->glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);

    // Initialize framebuffers
    gl->glGenFramebuffers(1, &framebuffer_);

    // Render buffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture_, 0);
    gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    // Initialize postprocess chain
    Postfx* fx = postfxChain_.front();

    if(!fx->initialize(width, height, samples))
        return false;

    fx->setInputTexture(renderTexture_);
    fx->setOutputFbo(0);    // Output to window

    return true;
}

void Renderer::render(AbstractScene* scene)
{
    // Pass 1
    // Render the geometry to a multisampled framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LESS);

    renderGeometry(scene);

    gl->glDisable(GL_DEPTH_TEST);

    // Pass 2
    // Render postprocess chain
    for(Postfx* fx : postfxChain_)
    {
        fx->render(quadVao_, 6);
    }
}

void Renderer::renderGeometry(AbstractScene* scene)
{
    QMatrix4x4 v = scene->activeCamera()->lookAt();
    VP_ = scene->activeCamera()->perspective() * v;

    technique_.enable();

    technique_.setEyeWorldPos(scene->activeCamera()->position());
    technique_.setTextureUnits(0, 1);
    technique_.setDirectionalLight(scene->queryDirectionalLight());
    technique_.setPointLights(scene->queryPointLights());
    technique_.setSpotLights(scene->querySpotLights());

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0, 0, 0, 0);

    gl->glEnable(GL_CULL_FACE);

    recursiveRender(&rootNode_, rootNode_.transformation());
}

void Renderer::recursiveRender(SceneNode* node, const QMatrix4x4& worldView)
{
    if(node == nullptr)
    {
        return;
    }

    RenderList renderList;

    // Render all entries
    for(int i = 0; i < node->numEntities(); ++i)
    {
        renderList.clear();

        Entity* entity = node->getEntity(i);
        if(entity != nullptr)
        {
            entity->updateRenderList(renderList);
            QMatrix4x4 modelView = worldView * node->transformation();

            // Node transformation
            technique_.setMVP(VP_ * modelView);
            technique_.setWorldView(modelView);

            // Render all renderables
            for(auto it = renderList.begin(); it != renderList.end(); ++it)
            {
                const Material::Ptr& material = (*it)->material();

                technique_.setMaterialAttributes(material->getAttributes());
                material->getTexture(Material::TEXTURE_DIFFUSE)->bind(GL_TEXTURE0);

                if((*it)->hasTangents())
                {
                    material->getTexture(Material::TEXTURE_NORMALS)->bind(GL_TEXTURE1);
                    technique_.setHasTangents(true);
                }

                else
                {
                    technique_.setHasTangents(false);
                }

                (*it)->render();
            }
        }
    }

    // Follow child nodes
    for(int i = 0; i < node->numChildren(); ++i)
    {
        SceneNode* inode = dynamic_cast<SceneNode*>(node->getChild(i));
        recursiveRender(inode, worldView * node->transformation());
    }
}