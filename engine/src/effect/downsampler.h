#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "common.h"
#include <QOpenGLFramebufferObject>
#include "resource/shaderprogram.h"

#include <vector>

#include "renderable/quad.h"

namespace Engine {

    class ResourceDespatcher;
    
namespace Effect {

class DownSampler
{
public:
    enum { SAMPLES = 4 };

    explicit DownSampler(ResourceDespatcher* despatcher);
    ~DownSampler();

    bool init(int width, int height, GLenum format);
    QOpenGLFramebufferObject* getSample(size_t n);

    // Expects the quad to be bound
    bool downSample(GLuint textureId, const Renderable::Quad& quad);

private:
    void destroy();

    ShaderProgram program_;
    std::vector<QOpenGLFramebufferObject*> fbos_;
};

}}

#endif //DOWNSAMPLER_H