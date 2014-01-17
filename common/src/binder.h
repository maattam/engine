//
//  Author   : Matti Määttä
//  Summary  : OpenGL Binder class designed to reduce unnecessary state changes for example when rendering
//             multiple meshes using the same textures.
//             NOTE: Using this class assumes that used OpenGL objects are bound to uniform locations, eg.
//             diffuse textures should always go to glActiveTexture(GL_TEXTURE0).
//

#ifndef BINDER_H
#define BINDER_H

#include "common.h"

#include <QMap>
#include <memory>

class Bindable;

class Binder
{
public:
    template<typename BindableDerived, typename... Args>
    static bool bind(BindableDerived& target, Args&&... args);

    template<typename BindableDerived, typename... Args>
    static bool bind(const std::shared_ptr<BindableDerived>& target, Args&&... args);

    template<typename BindableDerived, typename... Args>
    static bool bind(BindableDerived* target, Args&&... args);

    // Clears the state map.
    static void reset();

private:
    static bool test(Bindable& target, GLenum id);
    static bool test(Bindable& target);

    // Tests if value exists and is set.
    // Returns false if value was not already set.
    static bool testKey(GLenum key, GLuint value);

    static QMap<GLenum, GLuint> state_;
};

#include "binder.inl"

#endif // BINDER_H