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
    QOpenGLShader::ShaderTypeBit type() const;

private:
    QOpenGLShader::ShaderTypeBit type_;
    QByteArray data_;

    bool getShaderType(const QString& id, QOpenGLShader::ShaderTypeBit& type) const;
};

class Shader : public Resource<Shader, ShaderData>
{
public:
    Shader();
    Shader(const QString& name);
    virtual ~Shader();

    QOpenGLShader* get() const;

protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    QOpenGLShader* shader_;

};
    
};

#endif // SHADER_H