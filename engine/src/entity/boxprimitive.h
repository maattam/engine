#ifndef BOXPRIMITIVE_H
#define BOXPRIMITIVE_H

#include "entity.h"
#include "renderable/cube.h"
#include "material.h"

namespace Engine {

class BoxPrimitive : public Entity
{
public:
    BoxPrimitive(QOpenGLFunctions_4_2_Core* funcs);
    ~BoxPrimitive();

    virtual void updateRenderList(RenderList& list);

    void setMaterial(Material::Ptr& material);

private:
    Cube cube_;
};

}

#endif //BOXPRIMITIVE_H