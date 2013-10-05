#ifndef BOXPRIMITIVE_H
#define BOXPRIMITIVE_H

#include "entity.h"
#include "renderable/cube.h"
#include "material.h"

namespace Engine { namespace Entity {

class BoxPrimitive : public Entity
{
public:
    BoxPrimitive();
    ~BoxPrimitive();

    virtual void updateRenderList(RenderList& list);

    void setMaterial(const Material::Ptr& material);

private:
    Engine::Renderable::Cube cube_;
    Material::Ptr material_;
};

}}

#endif //BOXPRIMITIVE_H