#include "technique.h"

#include "resourcedespatcher.h"

#include <QDebug>

using namespace Engine::Technique;

Technique::Technique(Engine::ResourceDespatcher* despatcher)
    : despatcher_(despatcher)
{
}

Technique::~Technique()
{
}

bool Technique::enable()
{
    if(!program_->isLinked())
    {
        if(program_.ready())
        {
            init();
        }

        else
        {
            return false;
        }
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

void Technique::addShader(const QString& fileName, Engine::Shader::Type type)
{
    program_.addShader(despatcher_->get<Engine::Shader>(fileName, type));
}

Engine::ResourceDespatcher* Technique::despatcher() const
{
    return despatcher_;
}