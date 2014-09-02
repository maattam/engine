//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "shader.h"

#include <QDebug>

using namespace Engine;

Shader::Shader(Type type)
    : Resource(), shader_(nullptr), type_(type)
{
}

Shader::Shader(const QString& name, Type type)
    : Resource(name), shader_(nullptr), type_(type)
{
}

Shader::Shader(const QString& name, const ShaderData::DefineMap& defines, Type type)
    : Resource(name), shader_(nullptr), defines_(defines), type_(type)
{
}

Shader::~Shader()
{
    Shader::releaseResource();
}

bool Shader::initialiseData(const DataType& data)
{
    shader_ = new QOpenGLShader(type_);
    if(!shader_->compileSourceCode(data.data()))
    {
        return false;
    }
    
    return true;
}

void Shader::releaseResource()
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

Shader::Type Shader::type() const
{
    return type_;
}

void Shader::setNamedDefines(const ShaderData::DefineMap& defines)
{
    defines_ = defines;
}

Shader::ResourceDataPtr Shader::createData()
{
    std::shared_ptr<ShaderData> data(new ShaderData());
    data->setDefines(defines_);

    return data;
}