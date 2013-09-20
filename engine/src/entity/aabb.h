// Axis-aligned bounding box

#ifndef AABB_H
#define AABB_H

#include <QVector3D>
#include <vector>

namespace Engine { namespace Entity {

class AABB
{
public:
    AABB();
    AABB(const QVector3D& min, const QVector3D& max);

    bool resize(const AABB& other);
    bool resize(const std::vector<QVector3D>& vecs);
    void reset();

    bool empty() const;

private:
    QVector3D min_;
    QVector3D max_;
};

}}

#endif // AABB_H