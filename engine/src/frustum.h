#ifndef FRUSTUM_HH
#define FRUSTUM_HH

#include <QMatrix4x4>

#include "aabb.h"

namespace Engine {

// Tests whether the AABB is fully or partially inside the frustum
bool isInsideFrustum(const AABB& aabb, const QMatrix4x4& mvp);

// http://fgiesen.wordpress.com/2010/10/17/view-frustum-culling/
inline bool extentSignTest(const QVector3D& center, const QVector3D& extent, const QVector4D& plane);

}

#endif // FRUSTUM_HH