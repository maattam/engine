//
//  Author   : Matti Määttä
//  Summary  : 
//

template<GLenum Type>
Texture<Type>::Texture()
    : textureId_(0), width_(0), height_(0)
{
}

template<GLenum Type>
Texture<Type>::~Texture()
{
    remove();
}

template<GLenum Type>
void Texture<Type>::remove()
{
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
        textureId_ = 0;
    }
}

template<GLenum Type>
void Texture<Type>::texParameteri(GLenum pname, GLint target)
{
    gl->glTexParameteri(Type, pname, target);
}

template<GLenum Type>
void Texture<Type>::setFiltering(GLenum magFilter, GLenum minFilter)
{
    texParameteri(GL_TEXTURE_MAG_FILTER, magFilter);
    texParameteri(GL_TEXTURE_MIN_FILTER, minFilter);
}

template<GLenum Type>
void Texture<Type>::setWrap(GLenum wrap_s, GLenum wrap_t)
{
    texParameteri(GL_TEXTURE_WRAP_S, wrap_s);
    texParameteri(GL_TEXTURE_WRAP_T, wrap_t);
}

template<GLenum Type>
void Texture<Type>::generateMipmap()
{
    gl->glGenerateMipmap(Type);
}

template<GLenum Type>
bool Texture<Type>::bind()
{
    if(textureId_ == 0)
        return false;

    gl->glBindTexture(Target, textureId_);
    return true;
}

template<GLenum Type>
bool Texture<Type>::bind(GLenum target)
{
    gl->glActiveTexture(target);
    return bind();
}

template<GLenum Type>
GLuint Texture<Type>::handle() const
{
    return textureId_;
}

template<GLenum Type>
GLenum Texture<Type>::type() const
{
    return Type;
}

template<GLenum Type>
GLsizei Texture<Type>::height() const
{
    return height_;
}

template<GLenum Type>
GLsizei Texture<Type>::width() const
{
    return width_;
}

template<GLenum Type>
void Texture<Type>::setDimensions(GLsizei width, GLsizei height)
{
    width_ = width;
    height_ = height;
}