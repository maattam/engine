#ifndef SHADER_H
#define SHADER_H

#include "resource.h"

#include <QOpenGLShaderProgram>

namespace Engine {

class ShaderData : public ResourceData
{
public:
    ShaderData(ResourceDespatcher* despatcher);

    virtual bool load(const QString& fileName);

    const QByteArray& data() const;

private:
    QByteArray data_;
};

class Shader : public Resource<Shader, ShaderData>
{
public:
    typedef QOpenGLShader::ShaderTypeBit Type;

    Shader(Type type);
    Shader(const QString& name, Type type);
    virtual ~Shader();

    QOpenGLShader* get() const;

protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    QOpenGLShader* shader_;
    Type type_;

};
    
};

#endif // SHADER_H