// Renderable holds required OpenGL buffers in a VAO to describe a renderable.

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "common.h"
#include "entity/aabb.h"

#include <memory>

namespace Engine { namespace Renderable {

class Renderable
{
public:
    enum AttributeLocation { ATTRIB_VERTICES, ATTRIB_TEXCOORDS, ATTRIB_NORMALS, ATTRIB_TANGENTS };

    typedef std::shared_ptr<Renderable> Ptr;

    Renderable();
    virtual ~Renderable();

    virtual void render() const = 0;

    virtual bool hasTangents() const;

protected:
    bool bindVertexArray() const;
    void setTangents(bool tangents);

private:
    GLuint vertexArray_;
    bool hasTangents_;

    Renderable(const Renderable&);
    Renderable& operator=(const Renderable&);
};

}}

#endif //RENDERABLE_H