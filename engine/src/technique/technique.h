// Technique encapsulates a ShaderProgram -object so inherited classes can
// provide a convient interface for managing shader uniforms/procedures etc.

#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "shaderprogram.h"

#include <QString>
#include <QHash>

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

    // Returns the current QOpenGLShaderProgram encapsulated by the ShaderProgram
    // precondition: shaders added
    QOpenGLShaderProgram* program();

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

protected:
    // Called after the program has been linked for the first time
    // precondition: enable called successfully
    virtual bool init();

private:
    ShaderProgram program_;
    QHash<QString, int> uniforms_;
    QHash<QString, GLuint> subroutines_;

    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H