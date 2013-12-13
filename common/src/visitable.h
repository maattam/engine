// Acyclic visitor pattern.
// Base class has to inherit from BaseVisitable, and all visitable subclasses
// must declare the VISITABLE -macro.

#ifndef VISITABLE_H
#define VISITABLE_H

#include "visitor.h"

class BaseVisitable
{
public:
    virtual ~BaseVisitable() {};

    virtual void accept(BaseVisitor&) {};

protected:
    template<class T>
    static void acceptVisitor(T& visited, BaseVisitor& visitor);
};

#define VISITABLE \
    public: \
        virtual void accept(BaseVisitor& visitor) { \
            return acceptVisitor(*this, visitor); \
        }

#include "visitable.inl"

#endif // VISITABLE_H