#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>
#include <memory>
#include <QVector3D>

#include "resource/texture2D.h"

namespace Engine {

class ResourceDespatcher;

class Material
{
public:
    typedef std::shared_ptr<Material> Ptr;

    Material(ResourceDespatcher* despatcher);
    ~Material();

    enum TextureType { TEXTURE_DIFFUSE, TEXTURE_NORMALS, TEXTURE_SPECULAR, TEXTURE_COUNT };

    const Texture2D::Ptr& getTexture(TextureType type);
    void setTexture(TextureType type, Texture2D::Ptr& texture);

    bool bind();

    void setAmbientColor(const QVector3D& color);
    void setSpecularIntensity(float intensity);
    void setDiffuseColor(const QVector3D& color);
    void setShininess(float shininess);

    bool hasNormals() const;

    struct Attributes
    {
        QVector3D ambientColor;
        QVector3D diffuseColor;
        float shininess;
        float specularIntensity;

        Attributes()
        {
            ambientColor = QVector3D(0, 0, 0);
            diffuseColor = QVector3D(1, 1, 1);
            shininess = 5.0f;
            specularIntensity = 1.0f;
        }
    };

    const Attributes& getAttributes() const;

private:
    std::map<TextureType, Texture2D::Ptr> textures_;
    Attributes attributes_;
    ResourceDespatcher* despatcher_;

    void setTextureOptions(const Texture2D::Ptr& texture) const;

    Material(const Material&);
    Material& operator=(const Material&);
};

}

#endif //MATERIAL_H