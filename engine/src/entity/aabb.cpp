#include "aabb.h"

#include <qmath.h>

using namespace Engine;
using namespace Engine::Entity;

AABB::AABB()
{
}

AABB::AABB(const QVector3D& min, const QVector3D& max)
    : min_(min), max_(max)
{
}

bool AABB::resize(const AABB& other)
{
    if(resize(other.max_) || resize(other.min_))
        return true;

    return false;
}

void AABB::reset()
{
    min_ = max_ = QVector3D();
}

float AABB::size() const
{
    return min_.distanceToPoint(max_);
}

AABB AABB::operator*(const QMatrix4x4& rhs) const
{
    return AABB(rhs * min_, rhs * max_);
}

bool AABB::resize(const QVector3D& point)
{
    bool updated = false;

    // X
    if(min_.x() > point.x())
    {
        min_.setX(point.x());
        updated = true;
    }

    else if(max_.x() < point.x())
    {
        max_.setX(point.x());
        updated = true;
    }

    // Y
    if(min_.y() > point.y())
    {
        min_.setY(point.y());
        updated = true;
    }

    else if(max_.y() < point.y())
    {
        max_.setY(point.y());
        updated = true;
    }

    // Z
    if(min_.z() > point.z())
    {
        min_.setZ(point.z());
        updated = true;
    }

    else if(max_.z() < point.z())
    {
        max_.setZ(point.z());
        updated = true;
    }

    return updated;
}