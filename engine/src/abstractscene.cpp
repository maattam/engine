#include "abstractscene.h"

using namespace Engine;

AbstractScene::AbstractScene() : scene_(nullptr)
{
}

AbstractScene::~AbstractScene() {}

void AbstractScene::prepareScene(SceneNode* root)
{
    scene_ = root;
}

void AbstractScene::update(unsigned int)
{
}