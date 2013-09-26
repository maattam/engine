#include "technique.h"

#include "resource/shader.h"

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

void Technique::init()
{
}