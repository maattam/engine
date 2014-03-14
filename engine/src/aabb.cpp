//
//  Author   : Matti M‰‰tt‰
//  Summary  : 
//

#include "aabb.h"

using namespace Engine;

AABB::AABB()
{
}

AABB::AABB(const QVector3D& min, const QVector3D& max)
    : min_(min), max_(max)
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

	for(int i = 0; i < 3; ++i)
	{
		float& min = min_[i];
		float& max = max_[i];
		float compare = point[i];

		if(min > compare)
		{
			min = compare;
			updated = true;
		}

		else if(max < compare)
		{
			max = compare;
			updated = true;
		}
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