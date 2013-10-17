// Defines light data structures

#ifndef LIGHT_H
#define LIGHT_H

#include "entity.h"

#include <QVector3D>
#include <memory>

namespace Engine { namespace Entity {

class Light : public Entity
{
public:
    enum LightType { LIGHT_POINT, LIGHT_SPOT, LIGHT_DIRECTIONAL };

    typedef std::shared_ptr<Light> Ptr;
    Light(LightType type);

    struct Attenuation
    {
        float constant;
        float linear;
        float exp;
    } attenuation;

    QVector3D color;
    QVector3D direction;

    float ambientIntensity;
    float diffuseIntensity;
    float cutoff;

    LightType type() const;

    virtual void updateRenderList(RenderList& list);

private:
    LightType type_;
};

typedef std::pair<QVector3D, Light*> VisibleLight;

}}

#endif //LIGHT_H