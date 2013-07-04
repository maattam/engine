#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLFunctions_4_2_Core>

#include "material.h"

namespace Engine {

class Renderable
{
public:
    explicit Renderable(QOpenGLFunctions_4_2_Core* funcs);
    virtual ~Renderable();

    virtual void render() = 0;

    void setMaterial(Material::Ptr& material);
    const Material::Ptr& material();

protected:
    enum { ATTRIB_VERTICES, ATTRIB_TEXCOORDS, ATTRIB_NORMALS };

    QOpenGLFunctions_4_2_Core* gl;

    Material::Ptr material_;

    bool bindVertexArray();

private:
    GLuint vertexArray_;

    Renderable(const Renderable&);
    Renderable& operator=(const Renderable&);
};

}

#endif //RENDERABLE_H