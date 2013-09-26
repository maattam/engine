#include "shaderprogram.h"

#include <QDebug>

using namespace Engine;

ShaderProgram::ShaderProgram(QObject* parent)
    : QObject(parent), program_(nullptr)
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::addShader(const Shader::Ptr& shader)
{
    if(shader != nullptr)
    {
        shaders_.push_back(shader);

        connect(shader.get(), &Resource::released, this, &ShaderProgram::shaderReleased);
        connect(shader.get(), &Resource::initialized, this, &ShaderProgram::shaderCompiled);
    }
}

QOpenGLShaderProgram* ShaderProgram::operator->()
{
    return &program_;
}

QOpenGLShaderProgram& ShaderProgram::get()
{
    return program_;
}

void ShaderProgram::shaderReleased()
{
    // Release program
    program_.release();
    program_.removeAllShaders();
}

bool ShaderProgram::ready()
{
    if(program_.isLinked())
    {
        return true;
    }

    else if(!shadersLoaded())
    {
        return false;
    }

    else if(!program_.link())
    {
        qDebug() << __FUNCTION__ << "Failed to link program";
        return false;
    }

    return true;
}

bool ShaderProgram::shadersLoaded() const
{
    for(auto it = shaders_.begin(); it != shaders_.end(); ++it)
    {
        if(!(*it)->ready())
            return false;
    }

    return true;
}

void ShaderProgram::shaderCompiled()
{
    // Check if all shaders have compiled
    for(auto it = shaders_.begin(); it != shaders_.end(); ++it)
    {
        if((*it)->ready())
        {
            program_.addShader((*it)->get());
        }

        else
        {
            program_.removeAllShaders();
            return;
        }
    }
}