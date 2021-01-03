#pragma once


class Image
{
public:
    int width;
    int height;
    int channels;
    unsigned char* data;

    explicit Image(const char* path);
    ~Image();
};