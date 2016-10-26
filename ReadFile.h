#ifndef ReadFile_H__
#define ReadFile_H__

#include <vector>

std::vector<char> ReadFile(const char * filename);
std::vector<char> ReadFile(const wchar_t * filename);

#endif

