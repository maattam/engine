// Mesh is a collection of SubEntities, and represents an object that as been split into
// several submeshes.

#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "subentity.h"

#include <vector>

namespace Engine { namespace Entity {

class Mesh : public Entity
{
    typedef std::vector<SubEntity::Ptr> SubEntityVec;

public:
    Mesh();
    virtual ~Mesh();

    virtual void updateRenderList(RenderQueue& list);

    // Adds the given subEntity to this Mesh. The ownership is copied.
    // precondition: subEntity != nullptr
    void addSubEntity(const SubEntity::Ptr& subEntity);

    SubEntityVec::size_type numSubEntities() const;

    // precondition: index < numSubEntities
    const SubEntity::Ptr& subEntity(SubEntityVec::size_type index);

private:
    SubEntityVec entries_;

};

}}

#endif //MESH_H