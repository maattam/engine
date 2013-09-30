#include "boxprimitive.h"

using namespace Engine;
using namespace Engine::Entity;

BoxPrimitive::BoxPrimitive()
{
    updateAABB(AABB(QVector3D(-1, -1, -1), QVector3D(1, 1, 1)));
}

BoxPrimitive::~BoxPrimitive()
{
}

void BoxPrimitive::updateRenderList(RenderList& list)
{
    list.push_back(&cube_);
}

void BoxPrimitive::setMaterial(Material::Ptr& material)
{
    cube_.setMaterial(material);
}