#include "frustum.h"

using namespace Engine;

bool Engine::isInsideFrustum(const AABB& aabb, const QMatrix4x4& mvp)
{
    const QVector4D rowX = mvp.row(0);
    const QVector4D rowY = mvp.row(1);
    const QVector4D rowZ = mvp.row(2);
    const QVector4D rowW = mvp.row(3);

    const QVector3D center = aabb.center();
    const QVector3D extent = aabb.extent();

    // Left and right panes
    if(!extentSignTest(center, extent, rowW + rowX))
        return false;

    if(!extentSignTest(center, extent, rowW - rowX))
        return false;

    // Bottom and top planes
    if(!extentSignTest(center, extent, rowW + rowY))
        return false;

    if(!extentSignTest(center, extent, rowW - rowY))
        return false;

    // Near and far planes
    if(!extentSignTest(center, extent, rowW + rowZ))
        return false;

    if(!extentSignTest(center, extent, rowW - rowZ))
        return false;

    return true;    // TODO: Check if frustrum is outside the AABB
}

bool Engine::extentSignTest(const QVector3D& center, const QVector3D& extent, const QVector4D& plane)
{
    QVector3D normal = plane.toVector3D();
    QVector3D absNormal(qAbs(plane.x()), qAbs(plane.y()), qAbs(plane.z()));

    return QVector3D::dotProduct(center, normal) + QVector3D::dotProduct(extent, absNormal) > -plane.w();
}