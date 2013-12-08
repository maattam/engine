// Technique encapsulates a ShaderProgram -object so inherited classes can
// provide a convient interface for managing shader uniforms/procedures etc.

#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "shaderprogram.h"

namespace Engine { 
    
class Shader;

namespace Technique {

class Technique
{
public:
    Technique();
    virtual ~Technique();

    // Binds a shader to the current OpenGL context
    // precondition: shaders added
    // postcondition: shader bound and true returned
    virtual bool enable();

protected:
    // Returns the current QOpenGLShaderProgram encapsulated by the ShaderProgram
    // precondition: shaders added
    QOpenGLShaderProgram* program();

    // Adds a new shader to the program
    // postcondition: shader will be linked when enable is called
    void addShader(const Shader::Ptr& shader);

    // Called after the program has been linked for the first time
    // precondition: enable called successfully
    virtual void init() {};

private:
    ShaderProgram program_;

    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H