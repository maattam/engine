#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "common.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include <vector>

#include "renderable/quad.h"

namespace Engine { namespace Effect {

class DownSampler
{
public:
    enum { SAMPLES = 4 };

    explicit DownSampler(QOPENGL_FUNCTIONS* funcs);
    ~DownSampler();

    bool init(int width, int height, GLenum format);
    QOpenGLFramebufferObject* getSample(size_t n);

    // Expects the quad to be bound
    void downSample(GLuint textureId, const Renderable::Quad& quad);

private:
    void destroy();

    QOPENGL_FUNCTIONS* gl;
    QOpenGLShaderProgram program_;
    std::vector<QOpenGLFramebufferObject*> fbos_;

    DownSampler(const DownSampler&);
    DownSampler& operator=(const DownSampler&);
};

}}

#endif //DOWNSAMPLER_H