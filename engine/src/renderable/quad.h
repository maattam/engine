//
//  Author   : Matti Määttä
//  Summary  : Quad is a simple renderable which is used by render-to-texture effects.
//

#ifndef QUAD_H
#define QUAD_H

#include "renderable.h"

namespace Engine { namespace Renderable {

class Quad : public Renderable
{
public:
    explicit Quad();
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