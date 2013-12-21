// Engine render target texture object for Qt Scene Graph.

#ifndef RENDERTARGETITEM_H
#define RENDERTARGETITEM_H

#include <QQuickItem>

class QSGTexture;

namespace Engine { namespace Ui {

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

protected:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
    QSGTexture* texture_;
    bool textureChanged_;
};

}}

#endif // RENDERTARGETITEM_H