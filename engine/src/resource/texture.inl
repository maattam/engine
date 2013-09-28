template<GLenum Type>
Texture<Type>::Texture()
    : textureId_(0), mipmaps_(false)
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
    if(bind())
    {
        gl->glTexParameteri(Type, pname, target);
    }

    parametersi_.push_back(std::make_pair(pname, target));
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
void Texture<Type>::generateMipmaps()
{
    mipmaps_ = true;
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
bool Texture<Type>::bindActive(GLenum target)
{
    gl->glActiveTexture(target);
    return bind();
}

template<GLenum Type>
GLuint Texture<Type>::textureId() const
{
    return textureId_;
}

template<GLenum Type>
void Texture<Type>::setParameters()
{
    for(auto it = parametersi_.begin(); it != parametersi_.end(); ++it)
    {
        gl->glTexParameteri(Type, it->first, it->second);
    }

    if(mipmaps_)
    {
        gl->glGenerateMipmap(Type);
    }
}