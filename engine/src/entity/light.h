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
    };

    LightType type() const;

    void setColor(const QVector3D& color);
    const QVector3D& color() const;

    void setDirection(const QVector3D& direction);
    const QVector3D& direction() const;

    void setAmbientIntensity(float intensity);
    float ambientIntensity() const;

    void setDiffuseIntensity(float intensity);
    float diffuseIntensity() const;

    void setCutoff(float cutoff);
    float cutoff() const;

    void setAttenuationConstant(float attn);
    void setAttenuationLinear(float attn);
    void setAttenuationExp(float attn);

    const Attenuation& Light::attenuation() const;

    QVector3D position() const;

private:
    void calculateAABB();
    float maxSpotDistance() const;

    LightType type_;

    QVector3D color_;
    QVector3D direction_;

    float ambientIntensity_;
    float diffuseIntensity_;
    float cutoff_;

    Attenuation attenuation_;
};

}}

#endif //LIGHT_H