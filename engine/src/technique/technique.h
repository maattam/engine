#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "resource/resource.h"
#include "resource/shaderprogram.h"

namespace Engine { 
    
class Shader;

namespace Technique {

class Technique
{
public:
    Technique();
    virtual ~Technique();

    virtual bool enable();

protected:
    QOpenGLShaderProgram* program();

    void addShader(const Shader::Ptr& shader);
    virtual void init();

private:
    ShaderProgram program_;

    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H