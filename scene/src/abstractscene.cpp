#include "abstractscene.h"

#include "graph/scenenode.h"

using namespace Engine;

AbstractScene::AbstractScene()
{
}

AbstractScene::~AbstractScene() {}

Scene* AbstractScene::scene()
{
    return &scene_;
}

void AbstractScene::update(unsigned int elapsed)
{
};