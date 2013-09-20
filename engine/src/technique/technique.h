#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <QOpenGLShaderProgram>

namespace Engine { namespace Technique {

class Technique
{
public:
    Technique();
    virtual ~Technique();

    virtual void enable();

protected:
    QOpenGLShaderProgram program_;

private:
    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H