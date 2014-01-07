// Interface for OpenGL bindable objects.
// Derived bindable has to implement bool bind(...) function that does the actual binding.

#ifndef BINDABLE_H
#define BINDABLE_H

#include "common.h"

class Bindable
{
public:
    virtual ~Bindable() {};

    virtual GLuint handle() const = 0;
    virtual GLenum type() const = 0;
};

#endif // BINDABLE_H