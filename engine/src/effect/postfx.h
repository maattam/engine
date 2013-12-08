// Postprocess effect interface

#ifndef POSTFX_H
#define POSTFX_H

#include <QOpenGLContext>

namespace Engine { namespace Effect {

class Postfx
{
public:
    Postfx();
    virtual ~Postfx();

    virtual bool initialize(int width, int height, int samples) = 0;
    virtual void render() = 0;
    
    void setInputTexture(GLuint textureId);
    void setOutputFbo(GLuint framebufferId);

protected:
    GLuint outputFbo() const;
    GLuint inputTexture() const;

private:
    GLuint inputTexture_;
    GLuint outputFbo_;

    Postfx(const Postfx&);
    Postfx& operator=(const Postfx&);
};

}}

#endif //POSTFX_H