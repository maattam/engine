#include "rendertargetitem.h"

#include "rendertargettexture.h"

#include <QSGSimpleTextureNode>

using namespace Engine::Ui;

RenderTargetItem::RenderTargetItem(QQuickItem* parent)
    : QQuickItem(parent), texture_(nullptr), textureChanged_(false)
{
    setFlags(ItemHasContents);
    setSmooth(false);
    setAntialiasing(false);
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
    if(texture_ != nullptr)
    {
        delete texture_;
    }

    texture_ = new RenderTargetTexture(textureId, size);
    textureChanged_ = true;
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
    }

    if(textureChanged_)
    {
        node->setTexture(texture_);
        textureChanged_ = false;
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