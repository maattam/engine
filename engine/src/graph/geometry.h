// SubEntity holds reference to a material and renderable -pair.

#ifndef SUBENTITY_H
#define SUBENTITY_H

#include "sceneleaf.h"

#include "renderable/renderable.h"
#include "material.h"

namespace Engine { namespace Graph {

class Geometry : public SceneLeaf
{
public:
    typedef std::shared_ptr<Geometry> Ptr;

    Geometry(const Renderable::Renderable::Ptr& mesh, const Material::Ptr& material, const AABB& aabb);

    virtual void updateRenderList(RenderQueue& list);

    const Material::Ptr& material() const;

    // Sets the material used to render this sub entity
    // precondition: material != nullptr
    // postcondition: material ownership is copied
    void setMaterial(const Material::Ptr& material);

private:
    Renderable::Renderable::Ptr mesh_;
    Material::Ptr material_;
};

}}

#endif // SUBENTITY_H