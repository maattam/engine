#include "abstractscene.h"

#include "graph/scenenode.h"

using namespace Engine;

AbstractScene::AbstractScene() : scene_(nullptr)
{
}

AbstractScene::~AbstractScene() {}

void AbstractScene::prepareScene(Graph::SceneNode* root)
{
    scene_ = root;
}

void AbstractScene::update(unsigned int)
{
}

CubemapTexture* AbstractScene::skyboxTexture()
{
    return nullptr;
}

Renderable::Renderable* AbstractScene::skyboxMesh()
{
    return nullptr;
}