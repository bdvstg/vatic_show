#include "ReadFile.h"

#include <vector>
#include <iostream>

std::vector<char> ReadFile(const char * filename)
{
    std::FILE *file = std::fopen(filename, "rb");

    std::fseek(file, 0, SEEK_END); // SEEK_END means end of file
    unsigned int filesize = std::ftell(file);
    std::fseek(file, 0, SEEK_SET); // SEEK_SET means begin of file

    std::vector<char> buf(filesize);
    std::fread(buf.data(), 1, filesize, file);
    std::fclose(file);
    return buf;
}

std::vector<char> ReadFile(const wchar_t * filename)
{
    std::FILE *file = _wfopen(filename, L"rb");

    std::fseek(file, 0, SEEK_END); // SEEK_END means end of file
    unsigned int filesize = std::ftell(file);
    std::fseek(file, 0, SEEK_SET); // SEEK_SET means begin of file

    std::vector<char> buf(filesize);
    std::fread(buf.data(), 1, filesize, file);
    std::fclose(file);
    return buf;
}