#include "toymodel.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QStringList>

using namespace Engine;

ToyModel::ToyModel(QOpenGLFunctions_4_2_Core* funcs)
    : Renderable(funcs), vertexBuffer_(0), uvBuffer_(0), normalBuffer_(0),
    vertices_(0), indexBuffer_(0)
{
}

ToyModel::~ToyModel()
{
    if(vertexBuffer_ != 0)
    {
        gl->glDeleteBuffers(1, &vertexBuffer_);
    }

    if(uvBuffer_ != 0)
    {
        gl->glDeleteBuffers(1, &uvBuffer_);
    }

    if(normalBuffer_ != 0)
    {
        gl->glDeleteBuffers(1, &normalBuffer_);
    }
}

void ToyModel::render()
{
    if(!bindVertexArray() || vertices_ == 0)
        return;

    gl->glDrawArrays(GL_TRIANGLES, 0, vertices_);

    gl->glBindVertexArray(0);
}

bool ToyModel::loadFromFile(const QString& fn)
{
    // Something is already loaded
    if(vertexBuffer_ != 0)
        return false;

    QVector<QVector3D> vertices, normals, outVertices, outNormals;
    QVector<QVector2D> uvs, outUvs;

    QFile file(fn);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString arg;
        in >> arg;

        // Skip comments
        if(arg[0] == '#')
        {
            in.readLine();
            continue;
        }

        // Vertices
        if(arg == "v")
        {
            float x, y, z;
            in >> x >> y >> z;

            vertices.push_back(QVector3D(x, y, z));
        }

        // UVs
        else if(arg == "vt")
        {
            float u, v;
            in >> u >> v;

            uvs.push_back(QVector2D(u, v));
        }

        // Normals
        else if(arg == "vn")
        {
            float x, y, z;
            in >> x >> y >> z;

            normals.push_back(QVector3D(x, y, z));
        }

        // Faces f 5/1/1 1/2/1 4/3/1
        else if(arg == "f")
        {
            QString triplets[3];
            in >> triplets[0] >> triplets[1] >> triplets[2];

            for(int i = 0; i < 3; ++i)
            {
                QVector<int> indices;
                if(!readTriplet(triplets[i], indices))
                {
                    qDebug() << "ToyModel::Parse: Invalid indices: " << triplets[i];
                    return false;
                }

                if(indices[0] > vertices.size() || indices[1] > uvs.size() || indices[2] > normals.size())
                {
                    qDebug() << "ToyModel::Parse: Index out of range";
                    return false;
                }

                outVertices.push_back(vertices[indices[0] - 1]);
                outNormals.push_back(normals[indices[2] - 1]);

                if(indices[1] > 0)
                {
                    outUvs.push_back(uvs[indices[1] - 1]);
                }
            }
        }

        else
        {
            qDebug() << "ToyModel::Parse: Unhandled: " << arg << in.readLine();
            continue;
        }

        // Read past newline
        in.readLine();
    }
    return bufferData(outVertices, outUvs, outNormals);
}

bool ToyModel::readTriplet(const QString& triplet, QVector<int>& indices)
{
    QStringList inindices = triplet.split("/");
    indices.resize(3);

    if(inindices.size() != 3)
    {
        return false;
    }

    for(int i = 0; i < 3; ++i)
    {
        // No material
        if(i == 1 && inindices[1].isEmpty())
        {
            indices[1] = 0;
        }

        else
        {
            bool ok = true;
            int out = inindices[i].toInt(&ok);
            if(ok && out > 0)
            {
                indices[i] = out;
            }

            else
            {
                return false;
            }
        }
    }

    return true;
}

bool ToyModel::bufferData(const QVector<QVector3D>& vertices,
                          const QVector<QVector2D>& uvs, const QVector<QVector3D>& normals)
{
    if(!bindVertexArray())
        return false;

    gl->glGenBuffers(1, &vertexBuffer_);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), &vertices[0], GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(ATTRIB_VERTICES);
    gl->glVertexAttribPointer(ATTRIB_VERTICES, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    if(uvs.size() > 0)
    {
        gl->glGenBuffers(1, &uvBuffer_);
        gl->glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
        gl->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(QVector2D), &uvs[0], GL_STATIC_DRAW);

        gl->glEnableVertexAttribArray(ATTRIB_TEXCOORDS);
        gl->glVertexAttribPointer(ATTRIB_TEXCOORDS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    gl->glGenBuffers(1, &normalBuffer_);
    gl->glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
    gl->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(QVector3D), &normals[0], GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(ATTRIB_NORMALS);
    gl->glVertexAttribPointer(ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glBindVertexArray(0);

    vertices_ = vertices.size();

    return true;
}