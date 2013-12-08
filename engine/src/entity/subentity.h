// SubEntity holds reference to a material and submesh.
// Mesh is a collection of SubEntities.

#ifndef SUBENTITY_H
#define SUBENTITY_H

#include "entity.h"
#include "../renderable/submesh.h"
#include "material.h"

namespace Engine { namespace Entity {

class SubEntity : public Entity
{
public:
    typedef std::shared_ptr<SubEntity> Ptr;

    SubEntity(const Renderable::SubMesh::Ptr& subMesh, const Material::Ptr& material, const AABB& aabb);

    virtual void updateRenderList(RenderQueue& list);

    const Material::Ptr& material() const;

    // Sets the material used to render this sub entity
    // precondition: material != nullptr
    // postcondition: material ownership is copied
    void setMaterial(const Material::Ptr& material);

private:
    Renderable::SubMesh::Ptr subMesh_;
    Material::Ptr material_;
};

}}

#endif // SUBENTITY_H