#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "resource.h"

#include <QOpenGLShaderProgram>

namespace Engine { namespace Technique {

class Technique
{
public:
    Technique();
    virtual ~Technique();

    virtual bool enable();

protected:
    QOpenGLShaderProgram program_;

private:
    Technique(const Technique&);
    Technique& operator=(const Technique&);
};

}}

#endif //TECHNIQUE_H