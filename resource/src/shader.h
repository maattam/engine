//
//  Author   : Matti Määttä
//  Summary  : Shader wraps QOpenGLShader into Resource
//

#ifndef SHADER_H
#define SHADER_H

#include "resource.h"
#include "shaderdata.h"

#include <QOpenGLShader>

namespace Engine {

class Shader : public Resource<Shader, ShaderData>
{
public:
    typedef QOpenGLShader::ShaderTypeBit Type;

    Shader(Type type);
    Shader(const QString& name, Type type);
    virtual ~Shader();

    // Returns the managed QOpenGLShader object
    // postcondition: If the resource is not loaded, nullptr is returned
    QOpenGLShader* get() const;

    Type type() const;

    // Sets the named define values. Name defines must be in format
    // #define NAME <>, where <> denotes the named value.
    // Named defines must be defined before the shader is loaded/parsed.
    void setNamedDefines(const ShaderData::DefineMap& defines);

protected:
    virtual ResourceDataPtr createData();
    virtual bool initialiseData(const DataType& data);
    virtual void releaseResource();

private:
    QOpenGLShader* shader_;
    Type type_;
    QStringList additionalFiles_;
    ShaderData::DefineMap defines_;
};
    
};

#endif // SHADER_H