#ifndef FRUSTRUM_HH
#define FRUSTRUM_HH

#include <QMatrix4x4>

#include "aabb.h"

namespace Engine { namespace Entity {

class Frustrum
{
public:
    // Calculates frustrum from clip space transformation
    Frustrum(const QMatrix4x4& viewProjection);

    // Tests whether the AABB is fully or partially inside the frustrum
    bool contains(const AABB& aabb, const QMatrix4x4& modelView) const;

private:
    QMatrix4x4 viewProj_;

    static bool extentSignTest(const QVector3D& center, const QVector3D& extent, const QVector4D& plane);
};

}}

#endif // FRUSTRUM_HH