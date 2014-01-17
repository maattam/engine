//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "renderitemsorter.h"

#include "renderable/renderable.h"

using namespace Engine;

bool RenderItemSorter::operator()(const RenderQueue::RenderItem& first, const RenderQueue::RenderItem& second) const
{
    Material& mat1 = *first.material;
    Material& mat2 = *second.material;

    bool bumpsFirst = first.renderable->hasTangents() && mat1.hasTexture(Material::TEXTURE_NORMALS);
    bool bumpsSecond = second.renderable->hasTangents() && mat2.hasTexture(Material::TEXTURE_NORMALS);

    if(bumpsSecond && !bumpsFirst)
    {
        return true;
    }

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