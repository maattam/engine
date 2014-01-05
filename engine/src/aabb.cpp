#include "aabb.h"

#include <qmath.h>

using namespace Engine;

AABB::AABB() : first_(false)
{
}

AABB::AABB(const QVector3D& min, const QVector3D& max)
    : min_(min), max_(max), first_(true)
{
}

bool AABB::resize(const AABB& other)
{
    bool updated = false;

    if(resize(other.max_))
    {
        updated = true;
    }

    if(resize(other.min_))
    {
        updated = true;
    }

    return updated;
}

void AABB::reset(const QVector3D& min, const QVector3D& max)
{
    min_ = min;
    max_ = max;
}

float AABB::width() const
{
    return max_.x() - min_.x();
}

float AABB::height() const
{
    return max_.y() - min_.y();
}

float AABB::depth() const
{
    return max_.z() - min_.z();
}

bool AABB::resize(const QVector3D& point)
{
    bool updated = false;

    if(first_)
    {
        min_ = max_ = point;
        first_ = false;
        return true;
    }

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

QVector3D AABB::center() const
{
    return 0.5f * (max_ + min_);
}

QVector3D AABB::extent() const
{
    return 0.5f * (max_ - min_);
}