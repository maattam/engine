#include "technique.h"

using namespace Engine::Technique;

Technique::Technique()
{
}

Technique::~Technique()
{
}

bool Technique::enable()
{
    return program_.bind();
}