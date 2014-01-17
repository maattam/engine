//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "scenefactory.h"

#include "resourcedespatcher.h"
#include "sponzascene.h"
#include "basicscene.h"
#include "lightscene.h"

#include <QDebug>

SceneFactory::SceneFactory(Engine::ResourceDespatcher& despatcher)
    : despatcher_(despatcher)
{

}

FreeLookScene* SceneFactory::create(const QString& name)
{
    qDebug() << "Loading scene:" << name;

    if(name == "Sponza")
    {
        return new SponzaScene(despatcher_);
    }

    else if(name == "Shittyboxes")
    {
        return new BasicScene(despatcher_);
    }

    else if(name == "Lights")
    {
        return new LightScene(despatcher_);
    }

    return nullptr;
}