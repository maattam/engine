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
    bool shadersLoaded() const;

    QOpenGLShaderProgram* operator->();
    QOpenGLShaderProgram& get();

public slots:
    void shaderReleased();
    void shaderCompiled();

private:
    std::vector<Shader::Ptr> shaders_;
    QOpenGLShaderProgram program_;

    ShaderProgram(const ShaderProgram&);
    ShaderProgram& operator=(const ShaderProgram&);
};

}

#endif // PROGRAM