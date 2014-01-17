//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "shaderprogram.h"

#include <algorithm>
#include <QDebug>

using namespace Engine;

ShaderProgram::ShaderProgram(QObject* parent)
    : QObject(parent), program_(nullptr), needsLink_(false)
{
}

ShaderProgram::~ShaderProgram()
{
}

bool ShaderProgram::addShader(const Shader::Ptr& shader)
{
    if(shader == nullptr)
    {
        return false;
    }
    
    // If the shader is already compiled, we don't need to wait for signal
    if(shader->ready() && shader->get()->isCompiled())
    {
        if(!program_.addShader(shader->get()))
        {
            qDebug() << __FUNCTION__ << "Failed to add shader:" << shader->name();
            return false;
        }
    }

    shaders_.push_back(shader);
    needsLink_ = true;
    connect(shader.get(), &ResourceBase::released, this, &ShaderProgram::shaderReleased);

    return true;
}

QOpenGLShaderProgram* ShaderProgram::operator->()
{
    return &program_;
}

QOpenGLShaderProgram& ShaderProgram::get()
{
    return program_;
}

void ShaderProgram::shaderReleased(const QString& name)
{
    // Find shader
    auto iter = std::find_if(shaders_.begin(), shaders_.end(),
        [&name](const Shader::Ptr& shader) { return shader->name() == name; });

    Q_ASSERT(iter != shaders_.end());

    QList<QOpenGLShader*> linked = program_.shaders();
    if(linked.count((*iter)->get()) > 0)
    {
        program_.removeShader((*iter)->get());
        program_.release();

        needsLink_ = true;
    }
}

bool ShaderProgram::ready()
{
    if(needsLink_ && shadersLoaded())
    {
        needsLink_ = false;

        if(!program_.link())
        {
            qDebug() << __FUNCTION__ << "Failed to link program:";
            qDebug() << program_.log();

            return false;
        }
    }

    return program_.isLinked();
}

bool ShaderProgram::bind()
{
    return ready() && program_.bind();
}

bool ShaderProgram::shadersLoaded()
{
    if(shaders_.empty())
    {
        return false;
    }

    QList<QOpenGLShader*> linked = program_.shaders();
    for(const Shader::Ptr& shader : shaders_)
    {
        if(shader->ready() && linked.count(shader->get()) < 1)
        {
            if(!program_.addShader(shader->get()))
            {
                qDebug() << "Failed to link shader:" << shader->name();
                return false;
            }
        }
    }

    return shaders_.size() == program_.shaders().count();
}