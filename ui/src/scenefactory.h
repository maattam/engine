//
//  Author   : Matti Määttä
//  Summary  : Scene factory model used by scene presenters.
//

#ifndef SCENEFACTORY_H
#define SCENEFACTORY_H

#include <QString>
#include <QMap>
#include <QStringList>

namespace Engine {

class ResourceDespatcher;

namespace Ui {

class SceneController;

class SceneFactory
{
public:
    SceneFactory();

    void setDespatcher(ResourceDespatcher* despatcher);

    template<typename SceneType>
    void registerScene(const QString& name);

    // Returns list of registered scene types.
    QStringList sceneTypes() const;

    SceneController* create(const QString& name);

private:
    ResourceDespatcher* despatcher_;

    typedef SceneController* (*CreatorFunc)(ResourceDespatcher& despatcher);
    QMap<QString, CreatorFunc> sceneTypes_;
};

#include "scenefactory.inl"

}}

#endif // SCENEFACTORY_H