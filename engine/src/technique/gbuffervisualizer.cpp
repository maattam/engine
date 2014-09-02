//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "gbuffervisualizer.h"

#include "common.h"

using namespace Engine;
using namespace Engine::Technique;

GBufferVisualizer::GBufferVisualizer()
    : DSMaterialShader()
{
}

GBufferVisualizer::~GBufferVisualizer()
{
}

void GBufferVisualizer::outputTexture(TextureType type)
{
    GLuint id = GL_INVALID_INDEX;
    
    if(type == GB_POSITIONS)
    {
        id = cachedSubroutineLocation("outputPositions");
    }

    else if(type == GB_NORMALS)
    {
        id = cachedSubroutineLocation("outputNormals");
    }

    else
    {
        id = cachedSubroutineLocation("outputDiffuse");
    }

    gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &id);
}

bool GBufferVisualizer::init()
{
    if(!DSMaterialShader::init())
        return false;

    resolveSubroutineLocation("outputPositions", GL_FRAGMENT_SHADER);
    resolveSubroutineLocation("outputNormals", GL_FRAGMENT_SHADER);
    resolveSubroutineLocation("outputDiffuse", GL_FRAGMENT_SHADER);

    return true;
}