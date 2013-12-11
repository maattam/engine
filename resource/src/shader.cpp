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
    Shader::releaseData();
}

bool Shader::initialiseData(const DataType& data)
{
    shader_ = new QOpenGLShader(type_, this);

    if(!shader_->compileSourceCode(data.data()))
    {
       qWarning() << shader_->log();
       return false;
    }
    
    return true;
}

void Shader::releaseData()
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

ShaderData::ShaderData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher)
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

            else
            {
                QRegExp rx("\".+\"");
                if(rx.indexIn(line) != -1)
                {
                    QString fileRel = rx.cap().remove('\"');
                    QString dir = QFileInfo(fileName).dir().path();
                    if(!dir.isEmpty())
                    {
                        dir += "/";
                    }

                    if(!parseShader(dir + fileRel, result))
                    {
                        return false;
                    }
                }
            }

        }

        return true;
    }
}