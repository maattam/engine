// Common Light -class which can be a point, spot or a directional light

#ifndef LIGHT_H
#define LIGHT_H

#include "entity.h"

#include <QVector3D>
#include <memory>

namespace Engine { namespace Entity {

class Light : public Entity
{
    VISITABLE

public:
    enum LightType { LIGHT_POINT, LIGHT_SPOT, LIGHT_DIRECTIONAL };

    typedef std::shared_ptr<Light> Ptr;

    Light(LightType type);
    virtual ~Light();

    struct Attenuation
    {
        float constant;
        float linear;
        float quadratic;
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
    void setAttenuationQuadratic(float attn);

    const Attenuation& Light::attenuation() const;

    // Returns the light's world position derived from parentNode, or local position
    const QVector3D& position() const;
    void setPosition(const QVector3D& position);

    virtual void updateRenderList(RenderQueue& list) {}

    float cutoffDistance() const;

private:
    void calculateAABB();

    LightType type_;

    QVector3D color_;
    QVector3D direction_;
    QVector3D basePosition_;

    float ambientIntensity_;
    float diffuseIntensity_;
    float cutoff_;

    Attenuation attenuation_;
};

}}

#endif //LIGHT_H