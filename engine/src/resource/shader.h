#ifndef SHADER_H
#define SHADER_H

#include "resource.h"

#include <QOpenGLShaderProgram>
#include <memory>

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
    typedef std::shared_ptr<Shader> Ptr;

    Shader();
    Shader(const QString& name);
    virtual ~Shader();

    QOpenGLShader* get() const;

protected:
    virtual bool initialiseData(ShaderData& data);
    virtual void releaseData();

private:
    QOpenGLShader* shader_;

};
    
};

#endif // SHADER_H