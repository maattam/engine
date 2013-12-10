#include "shaderprogram.h"

#include <algorithm>
#include <QDebug>

using namespace Engine;

ShaderProgram::ShaderProgram(QObject* parent)
    : QObject(parent), program_(nullptr), compiledCount_(0), needsLink_(false)
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
        needsLink_ = true;

        connect(shader.get(), &ResourceBase::released, this, &ShaderProgram::shaderReleased);
        connect(shader.get(), &ResourceBase::initialized, this, &ShaderProgram::shaderCompiled);
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
        compiledCount_--;

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

bool ShaderProgram::shadersLoaded() const
{
    return !shaders_.empty() && compiledCount_ == shaders_.size();
}

void ShaderProgram::shaderCompiled(const QString& name)
{
    // Find shader
    auto iter = std::find_if(shaders_.begin(), shaders_.end(),
        [&name](const Shader::Ptr& shader) { return shader->name() == name; });

    Q_ASSERT(iter != shaders_.end());

    if(program_.addShader((*iter)->get()))
    {
        compiledCount_++;
    }

    else
    {
        qDebug() << __FUNCTION__ << "Failed to add shader:" << name;
    }
}