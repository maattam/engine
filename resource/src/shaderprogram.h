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

    // Adds a new shader to the program
    // precondition: shader != nullptr
    // postcondition: If the shader is already compiled, it is added immediately to the program,
    //                otherwise the program linkin is delayed until the resource is ready.
    //                Shader ownership is maintained.
    bool addShader(const Shader::Ptr& shader);

    bool ready();

    // Binds the shader, enabling draw calls
    bool bind();

    // Returns true if all the added shaders have been compiled.
    // Returns false if no shaders have been added.
    bool shadersLoaded() const;

    // Returns the encapsulated QOpenGLShaderProgram object.
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
};

}

#endif // PROGRAM