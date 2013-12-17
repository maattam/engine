#include "mesh.h"

using namespace Engine;
using namespace Engine::Entity;

Mesh::Mesh() : Entity()
{
}

Mesh::~Mesh()
{
}

void Mesh::updateRenderList(RenderQueue& list)
{
    for(SubEntity::Ptr& mesh : entries_)
        mesh->updateRenderList(list);
}

void Mesh::addSubEntity(const SubEntity::Ptr& subEntity)
{
    entries_.push_back(subEntity);

    AABB aabb = boundingBox();
    if(aabb.resize(subEntity->boundingBox()))
    {
        updateAABB(aabb);
    }
}

Mesh::SubEntityVec::size_type Mesh::numSubEntities() const
{
    return entries_.size();
}

const SubEntity::Ptr& Mesh::subEntity(SubEntityVec::size_type index)
{
    return entries_.at(index);
}