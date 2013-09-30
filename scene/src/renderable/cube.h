#ifndef CUBE_H
#define CUBE_H

#include "renderable.h"

namespace Engine { namespace Renderable {

class Cube : public Renderable
{
public:
    explicit Cube();
    ~Cube();

    virtual void render() const;

private:
    GLuint vertexBuffer_;
};

}}

#endif //CUBE_H