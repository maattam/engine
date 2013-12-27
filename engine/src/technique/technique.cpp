#include "technique.h"

#include "common.h"
#include "resourcedespatcher.h"

#include <QDebug>

using namespace Engine::Technique;

Technique::Technique()
{
}

Technique::~Technique()
{
}

bool Technique::enable()
{
    if(!program_->isLinked())
    {
        return program_.bind() && init();
    }

    return program_->bind();
}

QOpenGLShaderProgram* Technique::program()
{
    return &program_.get();
}

void Technique::addShader(const Engine::Shader::Ptr& shader)
{
    program_.addShader(shader);
}

int Technique::resolveUniformLocation(const QString& name)
{
    int id = program_->uniformLocation(name);
    if(id == -1)
    {
        qWarning() << "Failed to resolve uniform location:" << name;
    }

    else
    {
        uniforms_[name] = id;
    }

    return id;
}

GLuint Technique::resolveSubroutineLocation(const QString& name, GLenum type)
{
    GLuint id = gl->glGetSubroutineIndex(program()->programId(), type, name.toLatin1());
    if(id == GL_INVALID_INDEX)
    {
        qWarning() << "Failed to resolve subroutine uniform location:" << name;
    }

    else
    {
        subroutines_[name] = id;
    }

    return id;
}

int Technique::cachedUniformLocation(const QString& name) const
{
    int location = -1;

    auto iter = uniforms_.find(name);
    if(iter != uniforms_.end())
    {
        location = iter.value();
    }

    return location;
}

GLuint Technique::cachedSubroutineLocation(const QString& name) const
{
    GLuint location = GL_INVALID_INDEX;

    auto iter = subroutines_.find(name);
    if(iter != subroutines_.end())
    {
        location = iter.value();
    }

    return location;
}

bool Technique::useSubroutine(const QString& name, GLenum shaderType)
{
    GLenum location = cachedSubroutineLocation(name);
    if(location == GL_INVALID_INDEX)
    {
        if((location = resolveSubroutineLocation(name, shaderType)) == GL_INVALID_INDEX)
        {
            return false;
        }
    }

    // Avoid unnecessary subroutine swaps
    auto iter = boundSubroutines_.find(shaderType);
    if(iter == boundSubroutines_.end())
    {
        iter = boundSubroutines_.insert(shaderType, GL_INVALID_INDEX);
    }

    if(location != *iter)
    {
        gl->glUniformSubroutinesuiv(shaderType, 1, &location);
        *iter = location;
    }

    return true;
}