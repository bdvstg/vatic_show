#ifndef XML_VATIC_PASCAL_H__
#define XML_VATIC_PASCAL_H__

#include <string>
#include <vector>

typedef struct vatic_object_
{
    std::string name;
    int xmax;
    int xmin;
    int ymax;
    int ymin;
    // below is don't care now, but need write back to file
    int difficult;
    int occluded;
    std::string pose;
    int truncated;
} vatic_object;

vatic_object xml_vatic_new_object(
    const char* name = nullptr,
    int xmax = 50,
    int xmin = 0,
    int ymax = 50,
    int ymin = 0,
    int difficult = 0,
    int occluded = 0,
    const char* pose = "Unspecified",
    int truncated = 0);

std::vector<vatic_object> xml_vatic_pascal_parse(std::wstring filename);

void xml_vatic_pascal_modifyObjects(std::wstring filename,
    const std::vector<vatic_object> &objs);

std::vector<std::string> xml_vatic_get_names(
    const std::vector<vatic_object> &objs);

#endif

