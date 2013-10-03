#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "common.h"
#include <QOpenGLFramebufferObject>
#include "shaderprogram.h"

#include <vector>

#include "renderable/quad.h"

namespace Engine {

    class ResourceDespatcher;
    
namespace Effect {

class DownSampler
{
public:
    explicit DownSampler(ResourceDespatcher* despatcher);
    ~DownSampler();

    // Texture must have have maxLod mipmap levels allocated
    bool init(int width, int height, GLuint texture, GLuint maxLod);

    // Expects the quad to be bound
    // textureId must have maxLod mipmap levels as initialised
    bool downSample(GLuint textureId, const Renderable::Quad& quad);

private:
    void destroy();

    int width_;
    int height_;

    ShaderProgram program_;
    std::vector<GLuint> fbos_;
};

}}

#endif //DOWNSAMPLER_H