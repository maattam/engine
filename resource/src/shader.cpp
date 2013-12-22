#include "shader.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>

using namespace Engine;

namespace {
    // Handles #include -directives recursively
    bool parseShader(const QString& fileName, QByteArray& result);
}

Shader::Shader(QOpenGLShader::ShaderTypeBit type)
    : Resource(), shader_(nullptr), type_(type)
{
}

Shader::Shader(const QString& name, QOpenGLShader::ShaderTypeBit type)
    : Resource(name), shader_(nullptr), type_(type)
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

//
// ShaderData
//

ShaderData::ShaderData()
    : ResourceData()
{
}

bool ShaderData::load(const QString& fileName)
{
    return parseShader(fileName, data_);
}

const QByteArray& ShaderData::data() const
{
    return data_;
}

namespace {
    bool parseShader(const QString& fileName, QByteArray& result)
    {
        const QRegExp rxInclude("^\\s*#include\\s*\"[^\"\']+\"\\s*$");
        const QRegExp rxFileName("\".+\"");

        QFile file(fileName);
        QString line;

        if(!file.open(QIODevice::ReadOnly))
        {
            qWarning() << "Failed to open" << fileName << file.errorString();
            return false;
        }

        while(!file.atEnd())
        {
            line = file.readLine();
            if(rxInclude.indexIn(line) == -1)
            {
                result.append(line);
            }

            else if(rxFileName.indexIn(line) != -1)
            {
                QString fileRel = rxFileName.cap().remove('\"');
                QString dir = QFileInfo(fileName).dir().path();

                if(!dir.isEmpty())
                {
                    dir += QDir::separator();
                }

                if(!parseShader(dir + fileRel, result))
                {
                    return false;
                }
            }
        }

        return true;
    }
}