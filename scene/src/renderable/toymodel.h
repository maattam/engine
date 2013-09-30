#ifndef TOYMODEL_H
#define TOYMODEL_H

#include "renderable.h"

#include <QVector3D>
#include <QVector2D>
#include <QVector>

namespace Engine {

// Shitty .obj loader
class ToyModel : public Renderable
{
public:
    explicit ToyModel(QOpenGLFunctions_4_2_Core* funcs);
    ~ToyModel();

    bool loadFromFile(const QString& file);

    virtual void render();

private:
    GLuint vertexBuffer_;
    GLuint uvBuffer_;
    GLuint normalBuffer_;
    GLuint indexBuffer_;

    GLsizei vertices_;

    bool readTriplet(const QString& triplet, QVector<int>& indices);
    bool bufferData(const QVector<QVector3D>& vertices,
        const QVector<QVector2D>& uvs, const QVector<QVector3D>& normals);
};

}

#endif //TOYMODEL_H