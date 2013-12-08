// Helper class that encapsulates a QOpenGLShaderProgram and
// manages its shaders loading.

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "shader.h"

#include <vector>

namespace Engine {

class ShaderProgram : public QObject
{
    Q_OBJECT

public:
    explicit ShaderProgram(QObject* parent = nullptr);
    ~ShaderProgram();

    void addShader(const Shader::Ptr& shader);
    bool ready();
    bool bind();
    bool shadersLoaded() const;

    QOpenGLShaderProgram* operator->();
    QOpenGLShaderProgram& get();

public slots:
    void shaderReleased(const QString& name);
    void shaderCompiled(const QString& name);

private:
    std::vector<Shader::Ptr> shaders_;
    QOpenGLShaderProgram program_;
    unsigned int compiledCount_;
    bool needsLink_;

    ShaderProgram(const ShaderProgram&);
    ShaderProgram& operator=(const ShaderProgram&);
};

}

#endif // PROGRAM