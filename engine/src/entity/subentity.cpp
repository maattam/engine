#include "subentity.h"

#include "renderqueue.h"

using namespace Engine;
using namespace Engine::Entity;

SubEntity::SubEntity(const Renderable::SubMesh::Ptr& subMesh, const Material::Ptr& material, const AABB& aabb)
    : Entity(), subMesh_(subMesh), material_(material)
{
    updateAABB(aabb);
}

void SubEntity::updateRenderList(RenderQueue& list)
{
    if(subMesh_ != nullptr && material_ != nullptr)
    {
        list.addNode(material_.get(), subMesh_.get());
    }
}

const Material::Ptr& SubEntity::material() const
{
    return material_;
}

void SubEntity::setMaterial(const Material::Ptr& material)
{
    material_ = material;
}