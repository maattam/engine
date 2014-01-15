// Bridge class to allow QOpenGLFramebufferObject targets to be rendered as QSGTextures.

#ifndef RENDERTARGETTEXTURE_H
#define RENDERTARGETTEXTURE_H

#include <QSGTexture>

#include "common.h"

namespace Engine { namespace Ui {

class RenderTargetTexture : public QSGTexture
{
public:
    RenderTargetTexture();
    virtual ~RenderTargetTexture();

    virtual void bind();
    virtual bool hasAlphaChannel() const;
    virtual bool hasMipmaps() const;

    virtual int textureId() const;
    virtual QSize textureSize() const;

    void setSyncObject(GLsync sync);
    void update(int textureId, const QSize& size);

private:
    int textureId_;
    QSize size_;

    GLsync sync_;
};

}}

#endif // RENDERTARGETTEXTURE_H