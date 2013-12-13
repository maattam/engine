// Acyclic visitor pattern

#ifndef VISITOR_H
#define VISITOR_H

class BaseVisitor
{
public:
    virtual ~BaseVisitor() {};
};

template<class T>
class Visitor
{
public:
    virtual void visit(T&) = 0;
};

#endif // VISITOR_H