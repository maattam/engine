#include "renderitemsorter.h"

#include "texture2d.h"

using namespace Engine;

RenderItemSorter::RenderItemSorter(const QMatrix4x4& viewProj)
    : viewProj_(viewProj)
{
}

bool RenderItemSorter::operator()(const RenderQueue::RenderItem& first, const RenderQueue::RenderItem& second) const
{
    Material& mat1 = *first.material;
    Material& mat2 = *second.material;

    for(int i = 0; i < Material::TEXTURE_COUNT; ++i)
    {
        Material::TextureType type = static_cast<Material::TextureType>(i);

        // Group materials which share the same textures together to avoid unnecessary texture binding.
        if(mat1.getTexture(type) != mat2.getTexture(type))
        {
            return true;
        }
    }

    return false;
}