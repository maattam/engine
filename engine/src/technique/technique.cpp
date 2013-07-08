#include "technique.h"

using namespace Engine;

Technique::Technique()
{
}

Technique::~Technique()
{
}

void Technique::enable()
{
    program_.bind();
}