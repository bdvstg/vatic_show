#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

const std::vector<std::string> tryGetClasses(const std::wstring &rootPath)
{
    std::wstring filepath = rootPath + L"\\classes.txt";
    std::ifstream f;
    f.open(filepath, std::wifstream::in);
    std::vector<std::string> classes;
    while (f.is_open() && !f.eof())
    {
        std::string className;
        std::getline(f, className);
        if (className.size() > 0 && className.size() < 30)
            classes.push_back(className);
    }

    return classes;
}