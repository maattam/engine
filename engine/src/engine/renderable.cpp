#include "renderable.h"

using namespace Engine;

Renderable::Renderable(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), vertexArray_(0)
{
    gl->glGenVertexArrays(1, &vertexArray_);
}

Renderable::~Renderable()
{
    if(vertexArray_ != 0)
    {
        gl->glDeleteVertexArrays(1, &vertexArray_);
    }
}

bool Renderable::bindVertexArray()
{
    if(vertexArray_ == 0)
        return false;

    gl->glBindVertexArray(vertexArray_);
    return true;
}

void Renderable::setMaterial(Material::Ptr& material)
{
    material_ = material;
}

const Material::Ptr& Renderable::material()
{
    return material_;
}