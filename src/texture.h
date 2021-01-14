#pragma once

#include <string>

#include <glad/glad.h>


struct TextureOptions
{
    enum class Type
    {
        INVALID, AMBIENT, DIFFUSE, SPECULAR, NORMAL, DEPTH
    };

    GLenum  wrap_t   = GL_REPEAT;
    GLenum  wrap_s   = GL_REPEAT;
    GLenum  wrap_r   = GL_REPEAT;
    GLenum  min      = GL_LINEAR_MIPMAP_LINEAR;
    GLenum  mag      = GL_LINEAR;
    GLfloat lod_bias = 0.0f;
    GLenum  internal = GL_RGBA;
    Type    type     = Type::DIFFUSE;
};


class Image
{
public:
    int width     = 0;
    int height    = 0;
    int channels  = 0;
    const unsigned char* data = nullptr;

    std::string name      = "";
    std::string directory = "";

    static const Image from_raw(int width, int height, int channels, const unsigned char* data, const std::string& name, const std::string& directory);
    static const Image from_path(const std::string& name, const std::string& directory);
    static const Image empty();
};


struct Texture
{
    GLuint id        = 0;
    int    width     = 0;
    int    height    = 0;
    TextureOptions::Type type = TextureOptions::Type::DIFFUSE;
    int    dimension = 0;
    int    channels  = 0;
    float  shininess = 0;

    std::string name = "Uninitialized";

    explicit operator bool() const noexcept { return id; }
};

Texture CreateTexture2D(const Image& image, TextureOptions options = {});