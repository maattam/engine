#include "boxprimitive.h"

using namespace Engine;
using namespace Engine::Entity;

BoxPrimitive::BoxPrimitive()
{
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