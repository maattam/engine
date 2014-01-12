#include "resourcedata.h"

using namespace Engine;

ResourceData::ResourceData()
    : despatcher_(nullptr)
{
}

ResourceDespatcher* ResourceData::despatcher()
{
    return despatcher_;
}

void ResourceData::setDespatcher(ResourceDespatcher* despatcher)
{
    despatcher_ = despatcher;
}

QStringList ResourceData::queryFilesDebug() const
{
    return QStringList();
}