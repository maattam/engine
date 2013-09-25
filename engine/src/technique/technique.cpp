#include "technique.h"

#include "resource/shader.h"

#include <QDebug>

using namespace Engine::Technique;

Technique::Technique() : QObject(), program_(nullptr)
{
}

Technique::~Technique()
{
}

bool Technique::enable()
{
    if(program_ == nullptr || !program_->isLinked())
    {
        if(!linkShaders())
            return false;
    }

    return program_->bind();
}

void Technique::shaderReleased()
{
    // Release program
    if(program_ != nullptr)
    {
        delete program_;
        program_ = nullptr;
    }
}

void Technique::shaderCompiled()
{
    linkShaders();
}

bool Technique::linkShaders()
{
    if(program_ == nullptr)
    {
        program_ = new QOpenGLShaderProgram();
    }

    // Check if all shaders have compiled
    for(auto it = shaders_.begin(); it != shaders_.end(); ++it)
    {
        if((*it)->ready())
        {
            program_->addShader((*it)->get());
        }

        else
        {
            program_->removeAllShaders();
            return false;
        }
    }

    if(!program_->link())
    {
        qDebug() << __FUNCTION__ << "Failed to link program";
        return false;
    }

    else
    {
        init();
    }

    return true;
}

void Technique::addShader(const std::shared_ptr<Engine::Shader>& shader)
{
    if(shader != nullptr)
    {
        shaders_.push_back(shader);

        connect(shader.get(), &Resource::released, this, &Technique::shaderReleased);
        connect(shader.get(), &Resource::initialized, this, &Technique::shaderCompiled);
    }
}

QOpenGLShaderProgram* Technique::program() const
{
    return program_;
}

void Technique::init()
{
}