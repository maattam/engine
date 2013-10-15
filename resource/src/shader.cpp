#include "shader.h"

#include <QDebug>
#include <QFile>

using namespace Engine;

Shader::Shader(QOpenGLShader::ShaderTypeBit type)
    : Resource(), shader_(nullptr), type_(type)
{
}

Shader::Shader(const QString& name, QOpenGLShader::ShaderTypeBit type)
    : Resource(name), shader_(nullptr), type_(type)
{
}

Shader::~Shader()
{
    Shader::releaseData();
}

bool Shader::initialiseData(const DataType& data)
{
    shader_ = new QOpenGLShader(type_, this);
    bool ok = shader_->compileSourceCode(data.data());

    if(!ok)
    {
       qWarning() << shader_->log();
    }

    return ok;
}

void Shader::releaseData()
{
    if(shader_ != nullptr)
    {
        delete shader_;
        shader_ = nullptr;
    }
}

QOpenGLShader* Shader::get() const
{
    return shader_;
}

//
// ShaderData
//

ShaderData::ShaderData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher)
{
}

bool ShaderData::load(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    // TODO: Implement #include directive
    data_ = file.readAll();
    file.close();

    return true;
}

const QByteArray& ShaderData::data() const
{
    return data_;
}