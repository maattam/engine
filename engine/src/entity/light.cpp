#include "light.h"

#include <limits>
#include <qmath.h>

#include "mathelp.h"

using namespace Engine::Entity;

Light::Light(LightType type) : Entity(), type_(type)
{
    color_ = QVector3D(1.0f, 1.0f, 1.0f);

    ambientIntensity_ = 0.0f;
    diffuseIntensity_ = 1.0f;

    attenuation_.constant = 0.0f;
    attenuation_.linear = 0.0f;
    attenuation_.exp = 1.0f;

    cutoff_ = 30.0f;
    direction_ = UNIT_X;

    calculateAABB();
}

Light::LightType Light::type() const
{
    return type_;
}

void Light::updateRenderList(Engine::RenderList& list)
{
}

void Light::setColor(const QVector3D& color)
{
    color_ = color;
}

const QVector3D& Light::color() const
{
    return color_;
}

void Light::setDirection(const QVector3D& direction)
{
    direction_ = direction.normalized();
    calculateAABB();
}

const QVector3D& Light::direction() const
{
    return direction_;
}

void Light::setAmbientIntensity(float intensity)
{
    ambientIntensity_ = intensity;
}

float Light::ambientIntensity() const
{
    return ambientIntensity_;
}

void Light::setDiffuseIntensity(float intensity)
{
    diffuseIntensity_ = intensity;
    calculateAABB();
}

float Light::diffuseIntensity() const
{
    return diffuseIntensity_;
}

void Light::setCutoff(float cutoff)
{
    cutoff_ = cutoff;
    calculateAABB();
}

float Light::cutoff() const
{
    return cutoff_;
}

const Light::Attenuation& Light::attenuation() const
{
    return attenuation_;
}

void Light::setAttenuationConstant(float attn)
{
    attenuation_.constant = attn;
    calculateAABB();
}

void Light::setAttenuationLinear(float attn)
{
    attenuation_.linear = attn;
    calculateAABB();
}

void Light::setAttenuationExp(float attn)
{
    attenuation_.exp = attn;
    calculateAABB();
}

void Light::calculateAABB()
{
    AABB aabb;

    if(type_ == LIGHT_DIRECTIONAL)
    {
        // Directional light has infinite bounding volume
        const float inf = std::numeric_limits<float>::max();

        aabb.reset(QVector3D(-inf, -inf, -inf), QVector3D(inf, inf, inf));
    }

    else if(type_ == LIGHT_POINT)
    {
        // Point light spreads equally in all directions
        const float dist = maxSpotDistance();

        aabb.reset(QVector3D(-dist, -dist, -dist), QVector3D(dist, dist, dist));
    }

    // Spot light, inefficient brute force method
    else
    {
        const float dist = maxSpotDistance();
        const float height = qTan(qDegreesToRadians(cutoff_)) * dist;

        // Point light cone points, default orientation is facing +X
        const QVector3D conePoints[5] = { QVector3D(0, 0, 0),    // Cone origin
                                          QVector3D(dist, height, height),
                                          QVector3D(dist, -height, height),
                                          QVector3D(dist, height, -height),
                                          QVector3D(dist, -height, -height) };

        // Rotate cone extremes to match the light orientation
        QQuaternion rotation;

        // 180 degree turn; infite possibilites, choose one that works.
        if(QVector3D::dotProduct(UNIT_X, direction_) < -1.0f + 0.001f)
        {
            rotation = QQuaternion(0.0f, 0.0f, 1.0f, 0.0f);
        }

        else
        {
            rotation = rotationBetweenVectors(UNIT_X, direction_);
        }

        QMatrix4x4 rot;
        rot.rotate(rotation.normalized());

        for(size_t i = 0; i < 5; ++i)
        {
            aabb.resize(rot * conePoints[i]);
        }
    }

    updateAABB(aabb);
}

float Light::maxSpotDistance() const
{
    const float maxrgb = 256.0f;

    // Find max rgb
    float maxC = qMax(color_.x(), color_.y());
    maxC = qMax(maxC, color_.z());

    float root = attenuation_.linear * attenuation_.linear
        - 4 * attenuation_.exp * (attenuation_.constant - maxrgb * maxC * diffuseIntensity_);

    return (-attenuation_.linear + qSqrt(root)) / (2 * attenuation_.exp);
}