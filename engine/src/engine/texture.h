#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_4_2_Core>

#include <string>
#include <memory>

namespace Engine {

class Texture
{
public:
    typedef std::shared_ptr<Texture> Ptr;

    explicit Texture(QOpenGLFunctions_4_2_Core* funcs);
    ~Texture();

    void create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

    bool loadFromFile(const std::string& fileName);

    void setFiltering(GLenum magFilter, GLenum minFilter);
    void setWrap(GLenum wrap_s, GLenum wrap_t);
    void generateMipmaps();

    bool bind();
    bool bind(GLenum target);

    GLuint textureId();

    bool bound() const;

private:
    QOpenGLFunctions_4_2_Core* gl;
    GLuint textureId_;
    bool bound_;

    static GLuint boundTextureId_;

    Texture(const Texture&);
    Texture& operator=(const Texture&);
};

}

#endif //TEXTURE_H