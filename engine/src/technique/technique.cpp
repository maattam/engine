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

    if(!program_->bind())
    {
        return false;
    }

    activateUniformSubroutines();
    return true;
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

bool Technique::useSubroutine(const QString& name, const QString& index, GLenum shaderType)
{
    GLint uniform = -1;
    GLenum subroutineIndex = GL_INVALID_INDEX;

    // Resolve subroutine uniform location
    auto result = subroutineUniforms_.find(name);
    if(result != subroutineUniforms_.end())
    {
        uniform = result.value();
    }

    else
    {
        uniform = gl->glGetSubroutineUniformLocation(program()->programId(), shaderType, name.toLatin1());
        subroutineUniforms_[name] = uniform;
    }

    if(uniform == -1)
    {
        qDebug() << "Failed to resolve uniform subroutine location:" << name;
        return false;
    }

    // Resolve subroutine index
    subroutineIndex = cachedSubroutineLocation(index);
    if(subroutineIndex == GL_INVALID_INDEX)
    {
        if((subroutineIndex = resolveSubroutineLocation(index, shaderType)) == GL_INVALID_INDEX)
        {
            return false;
        }
    }

    // Avoid unnecessary subroutine swaps
    QVector<GLuint>& bindings = boundSubroutines_[shaderType];

    if(bindings.size() < uniform + 1)
    {
        bindings.resize(uniform + 1);
    }

    if(bindings[uniform] != subroutineIndex)
    {
        bindings[uniform] = subroutineIndex;
        activateUniformSubroutines();
    }

    return true;
}

void Technique::activateUniformSubroutines()
{
    for(auto it = boundSubroutines_.begin(); it != boundSubroutines_.end(); ++it)
    {
        gl->glUniformSubroutinesuiv(it.key(), it.value().size(), it.value().data());
    }
}