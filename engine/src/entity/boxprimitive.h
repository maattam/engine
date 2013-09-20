#ifndef BOXPRIMITIVE_H
#define BOXPRIMITIVE_H

#include "entity.h"
#include "renderable/cube.h"
#include "material.h"

namespace Engine { namespace Entity {

class BoxPrimitive : public Entity
{
public:
    BoxPrimitive(QOPENGL_FUNCTIONS* funcs);
    ~BoxPrimitive();

    virtual void updateRenderList(RenderList& list);

    void setMaterial(Material::Ptr& material);

private:
    Engine::Renderable::Cube cube_;
};

}}

#endif //BOXPRIMITIVE_H