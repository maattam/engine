#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "common.h"
#include "material.h"

namespace Engine { namespace Renderable {

class Renderable
{
public:
    explicit Renderable();
    virtual ~Renderable();

    virtual void render() const = 0;

    void setMaterial(Material::Ptr& material);
    const Material::Ptr& material();

    virtual bool hasTangents() const;

protected:
    enum { ATTRIB_VERTICES, ATTRIB_TEXCOORDS, ATTRIB_NORMALS, ATTRIB_TANGENTS };

    bool bindVertexArray() const;
    void setTangents(bool tangents);

private:
    GLuint vertexArray_;
    Material::Ptr material_;
    bool hasTangents_;

    Renderable(const Renderable&);
    Renderable& operator=(const Renderable&);
};

}}

#endif //RENDERABLE_H