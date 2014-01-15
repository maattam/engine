#include "rendertargetitem.h"

#include "rendertargettexture.h"

#include <QSGSimpleTextureNode>
#include <QQuickWindow>

using namespace Engine::Ui;

RenderTargetItem::RenderTargetItem(QQuickItem* parent)
    : QQuickItem(parent), texture_(nullptr)
{
    setFlags(ItemHasContents);
    setSmooth(false);
    setAntialiasing(false);

    texture_ = new RenderTargetTexture();
}

RenderTargetItem::~RenderTargetItem()
{
    if(texture_ != nullptr)
    {
        delete texture_;
    }
}

void RenderTargetItem::setTexture(int textureId, QSize size)
{
    texture_->update(textureId, size);
}

void RenderTargetItem::updateTexture(void* sync)
{
    texture_->setSyncObject(static_cast<GLsync>(sync));

    // Force both the RenderTragetItem and QQuickWindow to update to force repaint immediately.
    // The scene graph seems to wait ~5ms on average per frame on its own before the frame is rendered.
    // If window update is issued, the scene graph rendering is delayed ~2,3ms, which is still not ideal.
    update();
    window()->update();
}

QSGNode* RenderTargetItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    if(texture_ == nullptr)
    {
        return nullptr;
    }

    QSGSimpleTextureNode* node = static_cast<QSGSimpleTextureNode*>(oldNode);
    if(oldNode == nullptr)
    {
        node = new QSGSimpleTextureNode();
        node->setFiltering(QSGTexture::Nearest);
        node->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
        node->setTexture(texture_);
    }

    node->setRect(boundingRect());
    return node;
}

void RenderTargetItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    if(newGeometry.width() > 0 && newGeometry.height() > 0)
    {
        emit sizeChanged(QSize(newGeometry.width(), newGeometry.height()));
    }
}