#include "aabb.h"

#include <qmath.h>

using namespace Engine;
using namespace Engine::Entity;

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

void AABB::reset()
{
    min_ = max_ = QVector3D();
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

AABB AABB::operator*(const QMatrix4x4& mat) const
{
    float av, bv;

    AABB newAabb;
    newAabb.min_ = newAabb.max_ = mat.column(3).toVector3D();

    for(int i = 0; i < 3; ++i)
    {
        av = mat(i, 0) * min_.x();
        bv = mat(i, 0) * max_.x();

        if(av < bv)
        {
            newAabb.min_.setX(newAabb.min_.x() + av);
            newAabb.max_.setX(newAabb.max_.x() + bv);
        }

        else
        {
            newAabb.min_.setX(newAabb.min_.x() + bv);
            newAabb.max_.setX(newAabb.max_.x() + av);
        }

        av = mat(i, 1) * min_.y();
        bv = mat(i, 1) * max_.y();

        if(av < bv)
        {
            newAabb.min_.setY(newAabb.min_.y() + av);
            newAabb.max_.setY(newAabb.max_.y() + bv);
        }

        else
        {
            newAabb.min_.setY(newAabb.min_.y() + bv);
            newAabb.max_.setY(newAabb.max_.y() + av);
        }

        av = mat(i, 2) * min_.z();
        bv = mat(i, 2) * max_.z();

        if(av < bv)
        {
            newAabb.min_.setZ(newAabb.min_.z() + av);
            newAabb.max_.setZ(newAabb.max_.z() + bv);
        }

        else
        {
            newAabb.min_.setZ(newAabb.min_.z() + bv);
            newAabb.max_.setZ(newAabb.max_.z() + av);
        }
    }

    return newAabb;
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