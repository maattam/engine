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
    list.push_back(std::make_pair(material_.get(), &cube_));
}

void BoxPrimitive::setMaterial(const Material::Ptr& material)
{
    material_ = material;
}