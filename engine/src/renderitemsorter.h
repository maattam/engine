// RenderItemSorter sorts RenderItems based on material and textures.

#ifndef RENDERITEMSORTER_H
#define RENDERITEMSORTER_H

#include <QMatrix4x4>

#include "renderqueue.h"

namespace Engine {

class RenderItemSorter
{
public:
    RenderItemSorter(const QMatrix4x4& viewProj);

    bool operator()(const RenderQueue::RenderItem& first, const RenderQueue::RenderItem& second) const;

private:
    const QMatrix4x4& viewProj_;
};

}

#endif // RENDERITEMSORTER_H