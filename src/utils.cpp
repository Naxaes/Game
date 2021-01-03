#include "utils.h"

#include <fstream>

#include "debug.h"


std::unique_ptr<char> LoadFileToString(const char* path)
{
    std::ifstream file_stream(path);

    if (!file_stream.is_open())
    {
        WARNING("Couldn't open file '%s'.\n", path);
        return nullptr;
    }

    file_stream.seekg(0, std::ios::end);
    size_t size = file_stream.tellg();

    auto buffer = std::unique_ptr<char>((char* ) malloc(size + 1));

    file_stream.seekg(0);
    file_stream.read(buffer.get(), size);

    buffer.get()[size] = '\0';

    return std::move(buffer);
}
