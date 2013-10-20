// Axis-aligned bounding box

#ifndef AABB_H
#define AABB_H

#include <QVector3D>
#include <QMatrix4x4>

namespace Engine { namespace Entity {

class AABB
{
public:
    AABB();
    AABB(const QVector3D& min, const QVector3D& max);

    bool resize(const AABB& other);
    bool resize(const QVector3D& point);
    void reset(const QVector3D& min = QVector3D(), const QVector3D& max = QVector3D());

    float width() const;
    float height() const;
    float depth() const;

    QVector3D center() const;
    QVector3D extent() const;

private:
    QVector3D min_;
    QVector3D max_;

    bool first_;
};

}}

#endif // AABB_H