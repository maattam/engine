#ifndef SCENEFACTORY_H
#define SCENEFACTORY_H

#include <QString>

namespace Engine {
    class ResourceDespatcher;
}

class FreeLookScene;

class SceneFactory
{
public:
    explicit SceneFactory(Engine::ResourceDespatcher& despatcher);

    FreeLookScene* create(const QString& name);

private:
    Engine::ResourceDespatcher& despatcher_;
};

#endif // SCENEFACTORY_H