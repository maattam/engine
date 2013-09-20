#ifndef QUAD_H
#define QUAD_H

#include "renderable.h"

namespace Engine { namespace Renderable {

class Quad : public Renderable
{
public:
    explicit Quad(QOPENGL_FUNCTIONS* funcs);
    ~Quad();

    virtual void render() const;

    // Optimizations for instanced quad rendering
    void bindVaoDirect() const;
    void renderDirect() const;

private:
    GLuint vertexBuffer_;
};

}}

#endif //QUAD_H