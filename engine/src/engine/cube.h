#ifndef CUBE_H
#define CUBE_H

#include "renderable.h"

namespace Engine {

class Cube : public Renderable
{
public:
    explicit Cube(QOpenGLFunctions_4_2_Core* funcs);
    ~Cube();

    virtual void render();

private:
    GLuint vertexBuffer_;
};

}

#endif //CUBE_H