//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "scenefactory.h"

#include "resourcedespatcher.h"

#include <QDebug>

using namespace Engine;
using namespace Engine::Ui;

SceneFactory::SceneFactory()
    : despatcher_(nullptr)
{
}

void SceneFactory::setDespatcher(ResourceDespatcher* despatcher)
{
    despatcher_ = despatcher;
}

SceneController* SceneFactory::create(const QString& name)
{
    auto result = sceneTypes_.find(name);
    if(result != sceneTypes_.end())
    {
        qDebug() << "Loading scene:" << name;
        return result.value()(*despatcher_);
    }

    return nullptr;
}

QStringList SceneFactory::sceneTypes() const
{
    return sceneTypes_.keys();
}