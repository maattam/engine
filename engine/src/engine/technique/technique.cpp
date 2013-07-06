#include "technique.h"

using namespace Engine;

Technique::Technique()
{
}

Technique::~Technique()
{
}

bool Technique::init()
{
    // Nothing to initialize..
    return true;
}

void Technique::enable()
{
    program_.bind();
}