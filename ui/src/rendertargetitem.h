// Engine render target texture object for Qt Scene Graph.

#ifndef RENDERTARGETITEM_H
#define RENDERTARGETITEM_H

#include <QQuickItem>

namespace Engine { namespace Ui {

class RenderTargetTexture;

class RenderTargetItem : public QQuickItem
{
    Q_OBJECT

public:
    explicit RenderTargetItem(QQuickItem* parent = nullptr);
    virtual ~RenderTargetItem();

signals:
    void sizeChanged(QSize size);

public slots:
    // Maps the render target texture to this fbo.
    void setTexture(int textureId, QSize size);
    void updateTexture(void* sync);

protected:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
    RenderTargetTexture* texture_;
};

}}

#endif // RENDERTARGETITEM_H