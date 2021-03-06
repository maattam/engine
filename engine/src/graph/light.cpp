//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "light.h"

#include <QDebug>
#include <limits>
#include <qmath.h>

#include "mathelp.h"
#include "scenenode.h"
#include "camera.h"

using namespace Engine::Graph;

Light::Light(LightType type)
    : SceneLeaf(), type_(type), lightMask_(0)
{
    color_ = QVector3D(1.0f, 1.0f, 1.0f);

    ambientIntensity_ = 0.0f;
    diffuseIntensity_ = 1.0f;

    attenuation_.constant = 0.0f;
    attenuation_.linear = 0.0f;
    attenuation_.quadratic = 1.0f;

    angleInnerCone_ = angleOuterCone_ = 30.0f;
    direction_ = UNIT_X;

    calculateAABB();
}

Light::~Light()
{
}

unsigned int Light::lightMask() const
{
    return lightMask_;
}

void Light::setLightMask(unsigned int mask)
{
    lightMask_ = mask;
}

Light::LightType Light::type() const
{
    return type_;
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

void Light::setAngleOuterCone(float angle)
{
    angleOuterCone_ = angle;
    if(angleInnerCone_ < angleInnerCone_)
    {
        angleInnerCone_ = angle;
    }

    calculateAABB();
}

float Light::angleOuterCone() const
{
    return angleOuterCone_;
}

void Light::setAngleInnerCone(float angle)
{
    angleInnerCone_ = angle;
    if(angleInnerCone_ > angleOuterCone_)
    {
        angleOuterCone_ = angle;
        calculateAABB();
    }
}

float Light::angleInnerCone() const
{
    return angleInnerCone_;
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

void Light::setAttenuationQuadratic(float attn)
{
    attenuation_.quadratic = attn;
    calculateAABB();
}

void Light::calculateAABB()
{
    AABB aabb;

    if(type_ == LIGHT_DIRECTIONAL)
    {
        // Directional light has infinite bounding volume
		const float inf = std::numeric_limits<float>::max() / 2.0f;

        aabb.reset(QVector3D(-inf, -inf, -inf), QVector3D(inf, inf, inf));
    }

    else if(type_ == LIGHT_POINT)
    {
        // Point light spreads equally in all directions
        const float dist = cutoffDistance();

        aabb.reset(QVector3D(-dist, -dist, -dist), QVector3D(dist, dist, dist));
    }

    // Spot light, inefficient brute force method
    else
    {
        const float dist = cutoffDistance();
        const float height = qTan(qDegreesToRadians(angleOuterCone_)) * dist;

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

float Light::cutoffDistance() const
{
    // Calculate light's luminance using Rec 709 formula
    float luminance = QVector3D::dotProduct(color_, QVector3D(0.299f, 0.587f, 0.114f));
    luminance *= diffuseIntensity_;

    // Luminance where below is considered black
    const float cutoff = 0.005f;

    if(attenuation_.quadratic > 0)
    {
        float root = attenuation_.linear * attenuation_.linear
            - 4 * attenuation_.quadratic * (attenuation_.constant - luminance / cutoff);

        return (-attenuation_.linear + qSqrt(root)) / (2 * attenuation_.quadratic);
    }

    else if(attenuation_.linear > 0)
    {
        return (luminance / cutoff - attenuation_.constant) / attenuation_.linear;
    }

    else
    {
        return luminance / cutoff - attenuation_.constant;
    }
}

QVector3D Light::position() const
{
    // Update derived position is parent exists
    if(parentNode() != nullptr)
    {
        return parentNode()->worldPosition();
    }

    // Fallback
    return basePosition_;
}

void Light::setPosition(const QVector3D& position)
{
    basePosition_ = position;
}

std::shared_ptr<SceneLeaf> Light::cloneImpl() const
{
    return std::make_shared<Light>(*this);
}