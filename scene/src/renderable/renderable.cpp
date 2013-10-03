#include "renderable.h"

using namespace Engine::Renderable;

Renderable::Renderable()
    : vertexArray_(0), hasTangents_(false)
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

bool Renderable::hasTangents() const
{
    return hasTangents_;
}

void Renderable::setTangents(bool tangents)
{
    hasTangents_ = tangents;
}

bool Renderable::bindVertexArray() const
{
    if(vertexArray_ == 0)
        return false;

    gl->glBindVertexArray(vertexArray_);
    return true;
}

void Renderable::setMaterial(const Engine::Material::Ptr& material)
{
    material_ = material;
}

const Engine::Material::Ptr& Renderable::material()
{
    return material_;
}

void Renderable::renderWireframe() const
{
    // Disable culling so we can see the wireframe lines through objects
    gl->glDisable(GL_CULL_FACE);
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    render();

    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl->glEnable(GL_CULL_FACE);
}