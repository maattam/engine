#ifndef QUAD_H
#define QUAD_H

#include "renderable.h"

namespace Engine {

class Quad : public Renderable
{
public:
    explicit Quad(QOpenGLFunctions_4_2_Core* funcs);
    ~Quad();

    virtual void render() const;

    // Optimizations for instances quad rendering
    void bindVaoDirect() const;
    void renderDirect() const;

private:
    GLuint vertexBuffer_;
};

}

#endif //QUAD_H