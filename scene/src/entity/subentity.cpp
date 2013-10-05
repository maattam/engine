#include "subentity.h"

using namespace Engine;
using namespace Engine::Entity;

SubEntity::SubEntity(const Renderable::SubMesh::Ptr& subMesh, const Material::Ptr& material, const AABB& aabb)
    : Entity(), subMesh_(subMesh), material_(material)
{
    updateAABB(aabb);
}

void SubEntity::updateRenderList(RenderList& list)
{
    if(subMesh_ != nullptr && material_ != nullptr)
    {
        list.push_back(std::make_pair(material_.get(), subMesh_.get()));
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