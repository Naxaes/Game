#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>

#include <debug.h>


// TODO(ted): @LEAK We don't free this.
Image Image::from_path(const std::string& name, const std::string& directory)
{
    int width, height, channels;
    auto* data = stbi_load((directory + name).data(), &width, &height, &channels, 0);
    if (!data)
        WARNING("Couldn't load %s.", name.data());

    return { width, height, channels, data, name, directory };
}


Texture CreateTexture2D(const Image& image, TextureOptions options)
{
    GLenum format = 0;
    if (image.channels == 4)
        format = GL_RGBA;
    else if (image.channels == 3)
        format = GL_RGB;
    else
        ERROR("Unsupported number of channels %i in image %s.", image.channels, image.name.data());


    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Strategies for sampling the texture when it's magnified and minimized.
    //   * GL_NEAREST: chooses the closest pixel, resulting in a pixelated ("blocky") image.
    //   * GL_LINEAR: mixes the pixels around it, resulting in a blurry image.

    // We can also sample using mipmaps.
    //   * GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel size and uses nearest neighbor
    //                                interpolation for texture sampling.
    //   * GL_LINEAR_MIPMAP_NEAREST: takes the nearest mipmap level and samples using linear interpolation.
    //   * GL_NEAREST_MIPMAP_LINEAR: linearly interpolates between the two mipmaps that most closely match the size
    //                               of a pixel and samples via nearest neighbor interpolation.
    //   * GL_LINEAR_MIPMAP_LINEAR: linearly interpolates between the two closest mipmaps and samples the texture via
    //                            linear interpolation.

    // A common mistake is to set one of the mipmap filtering options as the magnification filter. This doesn't
    // have any effect since mipmaps are primarily used for when textures get downscaled: texture magnification
    // doesn't use mipmaps and giving it a mipmap filtering option will generate an OpenGL GL_INVALID_ENUM error code.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  1000);  // 1000 is the default.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, options.lod_bias);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.mag);

    // Strategies for when the texture coordinate is outside of the texture.
    //   * GL_REPEAT: The default behavior for textures. Repeats the texture image.
    //   * GL_MIRRORED_REPEAT: Same as GL_REPEAT but mirrors the image with each repeat.
    //   * GL_CLAMP_TO_EDGE: Clamps the coordinates between 0 and 1. The result is that higher coordinates become
    //                       clamped to the edge, resulting in a stretched edge pattern.
    //   * GL_CLAMP_TO_BORDER: Coordinates outside the range are now given a user-specified border color
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options.wrap_t);

//    vec4 border_color(1.0f, 1.0f, 0.0f, 1.0f);
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, options.internal, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);  // NOTE(ted): This has to be called after glTexImage2D!

    return { .id = texture, .type = options.type, .height = image.height, .width = image.width, .name = image.name, .channels = image.channels, .dimension = 2 };
}
