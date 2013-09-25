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
    if(shader_ != nullptr)
    {
        delete shader_;
        shader_ = nullptr;
    }
}

bool Shader::loadData(const QString& fileName)
{
    data_.clear();

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
    return true;
}

bool Shader::initializeData()
{
    if(shader_ != nullptr)
        delete shader_;

    shader_ = new QOpenGLShader(type_, this);
    bool ok = shader_->compileSourceCode(data_);

    data_.clear();

    if(!ok)
    {
       qWarning() << shader_->log();

       delete shader_;
       shader_ = nullptr;
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

bool Shader::getShaderType(const QString& id, QOpenGLShader::ShaderTypeBit& type) const
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

QOpenGLShader* Shader::get() const
{
    return shader_;
}