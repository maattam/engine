#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "resource/resource.h"

#include <vector>
#include <memory>

#include <QOpenGLShaderProgram>

namespace Engine { 
    
class Shader;

namespace Technique {

class Technique : public QObject
{
    Q_OBJECT

public:
    Technique();
    virtual ~Technique();

    virtual bool enable();

public slots:
    void shaderReleased();
    void shaderCompiled();

protected:
    QOpenGLShaderProgram* program() const;
    void addShader(const std::shared_ptr<Shader>& shader);
    virtual void init();

private:
    QOpenGLShaderProgram* program_;
    std::vector<std::shared_ptr<Shader>> shaders_;

    bool linkShaders();

    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H