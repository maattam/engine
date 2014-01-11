#include "geometry.h"

#include "renderqueue.h"

using namespace Engine;
using namespace Engine::Graph;

Geometry::Geometry(const Renderable::Renderable::Ptr& subMesh, const Material::Ptr& material)
    : SceneLeaf(), mesh_(subMesh), material_(material)
{
    updateAABB(subMesh->boundingBox());
}

void Geometry::updateRenderList(RenderQueue& list)
{
    Q_ASSERT(material_ && mesh_);
    list.addNode(material_.get(), mesh_.get());
}

const Material::Ptr& Geometry::material() const
{
    return material_;
}

void Geometry::setMaterial(const Material::Ptr& material)
{
    material_ = material;
}

std::shared_ptr<SceneLeaf> Geometry::clone() const
{
    return std::make_shared<Geometry>(*this);
}