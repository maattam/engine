#include "skyboxtechnique.h"

#include "resourcedespatcher.h"
#include "shader.h"
#include "cubemaptexture.h"
#include "entity/camera.h"

#include "common.h"

using namespace Engine;
using namespace Engine::Technique;

Skybox::Skybox(ResourceDespatcher* despatcher)
    : Technique(), mvpLocation_(-1), samplerLocation_(-1)
{
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment));
}

void Skybox::init()
{
    mvpLocation_ = program()->uniformLocation("gMVP");
    samplerLocation_ = program()->uniformLocation("gCubemapTexture");
}

void Skybox::render(Entity::Camera* camera, CubemapTexture* texture)
{
    if(camera == nullptr || texture == nullptr)
        return;

    QMatrix4x4 trans;
    trans.translate(camera->position());

    program()->setUniformValue(mvpLocation_, camera->worldView() * trans);
    program()->setUniformValue(samplerLocation_, 0);

    if(texture->bindActive(GL_TEXTURE0))
    {
        // We want to see the skybox texture from the inside
        gl->glCullFace(GL_FRONT);
        gl->glDepthFunc(GL_LEQUAL);

        mesh_.render();

        gl->glCullFace(GL_BACK);
    }
}