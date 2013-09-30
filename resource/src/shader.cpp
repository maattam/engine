#include "shader.h"

#include <QDebug>
#include <QFile>

using namespace Engine;

Shader::Shader()
    : Resource(), shader_(nullptr)
{
}

Shader::Shader(const QString& name)
    : Resource(name), shader_(nullptr)
{
}

Shader::~Shader()
{
    Shader::releaseData();
}

bool Shader::initialiseData(ShaderData& data)
{
    shader_ = new QOpenGLShader(data.type(), this);
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
    int typeIndex = fileName.lastIndexOf(".");
    if(!getShaderType(fileName.right(fileName.length() - typeIndex - 1), type_))
    {
        qWarning() << __FUNCTION__ << "Unrecognised shader type" << fileName;
        return false;
    }

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

QOpenGLShader::ShaderTypeBit ShaderData::type() const
{
    return type_;
}

bool ShaderData::getShaderType(const QString& id, QOpenGLShader::ShaderTypeBit& type) const
{
    if(id == "vert")
        type = QOpenGLShader::Vertex;

    else if(id == "frag")
        type = QOpenGLShader::Fragment;

    else
    {
        return false;
    }

    return true;
}