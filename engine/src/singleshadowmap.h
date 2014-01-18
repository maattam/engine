//
//  Author   : Matti Määttä
//  Summary  : Shadow map containing a single texture representing the scene depth
//             from the light's perspective.
//

#include "shadowmap.h"

#include "renderqueue.h"
#include "graph/camera.h"
#include "texture2d.h"

#ifndef SINGLESHADOWMAP_H
#define SINGLESHADOWMAP_H

namespace Engine {

class SingleShadowMap : public ShadowMap
{
public:
    SingleShadowMap();
    virtual ~SingleShadowMap();

    // Binds all textures starting from location.
    virtual bool bindTextures(GLenum location);

    virtual void setSize(const QSize& size);
    virtual const QSize& size();

    virtual bool create();

    virtual const QMatrix4x4& lightVP() const;
    void setLightVP(const QMatrix4x4& vp);

    bool bindFbo();
    GLuint fboHandle() const;

    // Returns the batch that contains the scene geometry within the light's frustum.
    RenderQueue& batch();

private:
    GLuint fbo_;
    Texture2D texture_;

    QMatrix4x4 lightVP_;
    RenderQueue queue_;
    QSize size_;
};

}

#endif // SINGLESHADOWMAP_H