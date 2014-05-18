//
//  Author   : Matti Määttä
//  Summary  : Material holds a collection of textures, the material shading technique and manages shader attributes.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include <array>
#include <memory>
#include <QVector3D>

namespace Engine {

class Texture2D;

class Material
{
public:
    typedef std::shared_ptr<Material> Ptr;
    typedef std::shared_ptr<Texture2D> TexturePtr;

    Material();
    ~Material();

    enum TextureType { TEXTURE_DIFFUSE, TEXTURE_NORMALS, TEXTURE_SPECULAR, TEXTURE_MASK, TEXTURE_SHININESS, TEXTURE_COUNT };

    // Render type determines the material's rendering stage.
    enum RenderType { RENDER_OPAQUE, RENDER_EMISSIVE, RENDER_TRANSPARENT, RENDER_COUNT };

    // Retrieves the texture associated with the type.
    // If the texture doesn't exists, a default texture is returned with the exception of
    // TEXTURE_NORMALS which doesn't have a default texture.
    // precondition: despatcher != nullptr
    // postcondition: a valid texture handle returned, or nullptr
    const TexturePtr& getTexture(TextureType type);

    // Associates texture with the given type. The texture's ownership is copied.
    // precondition: texture != nullptr
    // postcondition: texture is associated with the type
    void setTexture(TextureType type, const TexturePtr& texture);

    // Returns true if a texture has been associated with the given type.
    bool hasTexture(TextureType type) const;

    // Binds all textures in the same order as in TextureType beginning from GL_TEXTURE0
    // precondition: false if any of the textures can't be bound
    bool bind();

    void setAmbientColor(const QVector3D& color);
    void setSpecularIntensity(float intensity);
    void setDiffuseColor(const QVector3D& color);
    void setShininess(float shininess);
    void setAlpha(float alpha);

    struct Attributes
    {
        QVector3D ambientColor;
        QVector3D diffuseColor;
        float shininess;
        float specularIntensity;
        float alpha;

        Attributes();
    };

    const Attributes& attributes() const;
    void setAttributes(const Attributes& attrib);

    void setRenderType(RenderType type);
    RenderType renderType() const;

private:
    std::array<TexturePtr, TEXTURE_COUNT> textures_;
    Attributes attributes_;
    RenderType renderType_;

    void setTextureOptions(const TexturePtr& texture) const;

    Material(const Material&);
    Material& operator=(const Material&);
};

}

#endif //MATERIAL_H