//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "dsmaterialshader.h"

#include "common.h"
#include "gbuffer.h"

using namespace Engine;
using namespace Engine::Technique;

DSMaterialShader::DSMaterialShader()
    : Technique(), gbuffer_(nullptr)
{
}

DSMaterialShader::~DSMaterialShader()
{
}

void DSMaterialShader::setGBuffer(GBuffer const* gbuffer)
{
    gbuffer_ = gbuffer;
}

void DSMaterialShader::setProjMatrix(const QMatrix4x4& proj)
{
    setUniformValue("invPersProj", proj.inverted());
}

void DSMaterialShader::setViewport(const QRect& viewport)
{
    viewport_ = QVector4D(viewport.x(), viewport.y(), viewport.width(), viewport.height());

    if(program()->isLinked())
    {
        setUniformValue("viewport", viewport_);
    }
}

bool DSMaterialShader::init()
{
    if(gbuffer_ == nullptr)
    {
        return false;
    }

    // Set texture samplers
    int i = 0;
    for(const QString& texture : gbuffer_->textures())
    {
        if(!setUniformValue(texture + "Data", i++))
        {
            return false;
        }
    }

    if(!setUniformValue("viewport", viewport_))
    {
        return false;
    }

    return resolveUniformLocation("invPersProj") != -1;
}

GBuffer const* DSMaterialShader::gbuffer()
{
    return gbuffer_;
}