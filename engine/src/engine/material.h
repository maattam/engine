#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <map>

#include <QVector3D>
#include <QOpenGLFunctions_4_2_Core>

#include "texture.h"

namespace Engine {

class Material
{
public:
    typedef std::shared_ptr<Material> Ptr;

    Material(QOpenGLFunctions_4_2_Core* funcs);
    ~Material();

    enum TextureType { TEXTURE_DIFFUSE, TEXTURE_NORMALS, TEXTURE_SPECULAR };

    const Texture::Ptr& getTexture(TextureType type);
    void setTexture(TextureType type, Texture::Ptr& texture);

    void setAmbientColor(const QVector3D& color);
    void setSpecularColor(const QVector3D& color);
    void setDiffuseColor(const QVector3D& color);

    struct Attributes
    {
        QVector3D ambientColor;
        QVector3D specularColor;
        QVector3D diffuseColor;

        Attributes()
        {
            ambientColor = QVector3D(0, 0, 0);
            specularColor = QVector3D(0, 0, 0);
            diffuseColor = QVector3D(1, 1, 1);
        }
    };

    const Attributes& getAttributes() const;

private:
    QOpenGLFunctions_4_2_Core* gl;

    std::map<TextureType, Texture::Ptr> textures_;
    Attributes attributes_;

    bool loadNullTexture(Texture::Ptr& texture) const;
    void setTextureOptions(const Texture::Ptr& texture) const;

    Material(const Material&);
    Material& operator=(const Material&);
};

}

#endif //MATERIAL_H