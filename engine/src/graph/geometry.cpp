#include "geometry.h"

#include "renderqueue.h"

using namespace Engine;
using namespace Engine::Graph;

Geometry::Geometry(const Renderable::Renderable::Ptr& subMesh, const Material::Ptr& material, const AABB& aabb)
    : SceneLeaf(), mesh_(subMesh), material_(material)
{
    updateAABB(aabb);
}

void Geometry::updateRenderList(RenderQueue& list)
{
    if(mesh_ != nullptr && material_ != nullptr)
    {
        list.addNode(material_.get(), mesh_.get());
    }
}

const Material::Ptr& Geometry::material() const
{
    return material_;
}

void Geometry::setMaterial(const Material::Ptr& material)
{
    material_ = material;
}