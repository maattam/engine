#ifndef SHADERDATA_H
#define SHADERDATA_H

#include "resourcedata.h"

#include <QStringList>
#include <QMap>
#include <QVariant>

namespace Engine {

class ShaderData : public ResourceData
{
public:
    ShaderData();

    virtual bool load(const QString& fileName);

    const QByteArray& data() const;

    // Reimplement to provide additional triggers for file watching
    virtual QStringList queryFilesDebug() const;

    typedef QMap<QString, QVariant> DefineMap;
    void setDefines(const DefineMap& map);

private:
    QByteArray data_;
    QStringList additionalFiles_;
    DefineMap defines_;
};

}

#endif // SHADERDATA_H