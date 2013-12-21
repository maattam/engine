// Bridge class to allow QOpenGLFramebufferObject targets to be rendered as QSGTextures.

#ifndef RENDERTARGETTEXTURE_H
#define RENDERTARGETTEXTURE_H

#include <QSGTexture>

namespace Engine { namespace Ui {

class RenderTargetTexture : public QSGTexture
{
public:
    RenderTargetTexture(int textureId, const QSize& size);
    virtual ~RenderTargetTexture();

    virtual void bind();
    virtual bool hasAlphaChannel() const;
    virtual bool hasMipmaps() const;

    virtual int textureId() const;
    virtual QSize textureSize() const;

private:
    int textureId_;
    QSize size_;
};

}}

#endif // RENDERTARGETTEXTURE_H