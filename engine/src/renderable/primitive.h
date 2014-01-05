// Static factory method for accessing immutable primitives such as cubes or quads.

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <memory>

namespace Engine { namespace Renderable {

template<typename RenderableType>
class Primitive
{
public:
    static std::shared_ptr<RenderableType> instance();

private:
    static std::weak_ptr<RenderableType> instance_;
};

#include "primitive.inl"

}}

#endif // PRIMITIVE_H