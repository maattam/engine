#ifndef SHADER_H
#define SHADER_H

#include "resource.h"

#include <QOpenGLShaderProgram>
#include <memory>

namespace Engine {

class Shader : public Resource
{
public:
    typedef std::shared_ptr<Shader> Ptr;

    Shader();
    Shader(const QString& name);
    virtual ~Shader();

    QOpenGLShader* get() const;

protected:
    virtual bool loadData(const QString& fileName);
    virtual bool initializeData();
    virtual void releaseData();

private:
    QOpenGLShader* shader_;
    QOpenGLShader::ShaderTypeBit type_;
    QByteArray data_;

    bool getShaderType(const QString& id, QOpenGLShader::ShaderTypeBit& type) const;

};
    
};

#endif // SHADER_H