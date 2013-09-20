#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <map>

#include <QVector3D>
#include "common.h"

#include "texture.h"

namespace Engine {

class Material
{
public:
    typedef std::shared_ptr<Material> Ptr;

    Material(QOPENGL_FUNCTIONS* funcs);
    ~Material();

    enum TextureType { TEXTURE_DIFFUSE, TEXTURE_NORMALS, TEXTURE_SPECULAR };

    const Texture::Ptr& getTexture(TextureType type);
    void setTexture(TextureType type, Texture::Ptr& texture);

    void setAmbientColor(const QVector3D& color);
    void setSpecularIntensity(float intensity);
    void setDiffuseColor(const QVector3D& color);
    void setShininess(float shininess);

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
    QOPENGL_FUNCTIONS* gl;

    std::map<TextureType, Texture::Ptr> textures_;
    Attributes attributes_;

    bool loadNullTexture(Texture::Ptr& texture) const;
    void setTextureOptions(const Texture::Ptr& texture) const;

    Material(const Material&);
    Material& operator=(const Material&);
};

}

#endif //MATERIAL_H