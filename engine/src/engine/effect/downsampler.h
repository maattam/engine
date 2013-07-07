#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include <vector>

#include "../quad.h"

namespace Engine {

class DownSampler
{
public:
    enum { SAMPLES = 4 };

    explicit DownSampler(QOpenGLFunctions_4_2_Core* funcs);
    ~DownSampler();

    bool init(int width, int height, GLenum format);
    QOpenGLFramebufferObject* getSample(size_t n);

    // Expects the quad to be bound
    void downSample(GLuint textureId, const Quad& quad);

private:
    void destroy();

    QOpenGLFunctions_4_2_Core* gl;
    QOpenGLShaderProgram program_;
    std::vector<QOpenGLFramebufferObject*> fbos_;

    DownSampler(const DownSampler&);
    DownSampler& operator=(const DownSampler&);
};

}

#endif //DOWNSAMPLER_H