#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_resize.h>


Image::Image(const char* path)
{
    this->data = stbi_load(path, &this->width, &this->height, &this->channels, 0);
}

Image::~Image()
{
    stbi_image_free(data);
}
