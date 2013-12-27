// Technique encapsulates a ShaderProgram -object so inherited classes can
// provide a convient interface for managing shader uniforms/procedures etc.

#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "shaderprogram.h"

#include <QString>
#include <QHash>
#include <QMap>

namespace Engine { 
    
class Shader;

namespace Technique {

class Technique
{
public:
    Technique();
    virtual ~Technique();

    // Binds a shader to the current OpenGL context
    // precondition: shaders added, init was successful
    // postcondition: shader bound and true returned
    virtual bool enable();

    // Adds a new shader to the program
    // precondition: shader not null
    // postcondition: shader will be linked when enable is called
    void addShader(const Shader::Ptr& shader);

    // A convenience function which uses the cached uniform name if resolved,
    // or resolves and caches to location before setting the value.
    // precondition: Technique is enabled
    // postcondition: Value set and name cached when true is returned.
    template<typename T>
    bool setUniformValue(const QString& name, T&& value);

    // A convenience function which binds the cached subroutine name or resolves it
    // before setting the subroutine uniform.
    // precondition: Technique is enabled
    bool useSubroutine(const QString& name, GLenum shaderType);

    // Returns the current QOpenGLShaderProgram encapsulated by the ShaderProgram
    // precondition: shaders added
    QOpenGLShaderProgram* program();

protected:
    // Called after the program has been linked successfully for the first time
    // precondition: enable called successfully
    virtual bool init() = 0;

    // Returns a cached uniform location from the hash.
    // The uniform must have been registered through resolveUniformLocation, or
    // resolveSubroutineLocation.
    // If the uniform is not found, -1 is returned.
    int cachedUniformLocation(const QString& name) const;
    GLuint cachedSubroutineLocation(const QString& name) const;

    // Attemps to locate uniforms from the bound program
    // Returns -1 if given uniform is invalid.
    // precondition: Technique has been enabled successfully
    int resolveUniformLocation(const QString& name);
    GLuint resolveSubroutineLocation(const QString& name, GLenum shaderType);

private:
    ShaderProgram program_;
    QMap<GLenum, GLuint> boundSubroutines_;

    QHash<QString, int> uniforms_;
    QHash<QString, GLuint> subroutines_;

    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

#include "technique.inl"

}}

#endif //TECHNIQUE_H