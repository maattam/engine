#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "common.h"
#include "renderable/quad.h"

#include <vector>
#include <memory>

namespace Engine {

class ResourceDespatcher;

namespace Technique {
    class BlurFilter;
}
    
namespace Effect {

class DownSampler
{
public:
    explicit DownSampler(ResourceDespatcher* despatcher);
    ~DownSampler();

    // Texture must have have maxLod mipmap levels allocated
    bool initialise(int width, int height, GLuint texture, GLuint maxLod);

    // Performs the downsampling to the given textureId.
    // precondition: initialise is called and must match the given textureId.
    //               quad must be bound for direct rendering.
    bool downSample(GLuint textureId, const Renderable::Quad& quad);

    typedef std::shared_ptr<Technique::BlurFilter> FilterPtr;

    // Sets the used blur technique.
    // precondition: filter != nullptr
    // postcondition: Filter ownership is maintained.
    void setBlurFilter(const FilterPtr& filter);

private:
    void destroy();

    int width_;
    int height_;

    FilterPtr filter_;
    std::vector<GLuint> fbos_;
};

}}

#endif //DOWNSAMPLER_H