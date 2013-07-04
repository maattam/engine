#include "boxprimitive.h"

using namespace Engine;

BoxPrimitive::BoxPrimitive(QOpenGLFunctions_4_2_Core* funcs)
    : cube_(funcs)
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