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

    DownSampler();
    ~DownSampler();

    bool init(int width, int height, GLenum format);
    QOpenGLFramebufferObject* getSample(size_t n);

    // Expects the quad to be bound
    void downSample(GLuint textureId, const Renderable::Quad& quad);

private:
    void destroy();

    QOpenGLShaderProgram program_;
    std::vector<QOpenGLFramebufferObject*> fbos_;

    DownSampler(const DownSampler&);
    DownSampler& operator=(const DownSampler&);
};

}}

#endif //DOWNSAMPLER_H