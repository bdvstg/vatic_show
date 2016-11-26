#include "fileManage.h"

#include "FileDialogWin.h"

// is filenames(without extname) inside foldXml and foldJpg is all same
static bool isConsistent(filenames_t xmls, filenames_t jpgs)
{
    if (xmls.size() != jpgs.size())
        return false;

    for (int i = 0; i < (int)xmls.size(); i++)
    {
        auto & xmlname = xmls[i];
        auto & jpgname = jpgs[i];
        // delete ext name
        xmlname.erase(xmlname.length() - 4, 4);
        jpgname.erase(xmlname.length() - 4, 4);
        // compare
        if (xmlname.compare(jpgname) != 0)
            return false; // not equal
    }

    return true;
}

// convert filenames to more compact format
// it only hold basename and which folder belong
static fileManage::frames_t toFrames(
    const filenames_t & xmls,
    const filenames_t & deletedXmls)
{
    fileManage::frames_t frames;
    // belong folder normal
    for each (auto name in xmls)
    {
        name.erase(name.length() - 4, 4); // remove extname
        fileManage::frame_t frame = { name, false };
        frames.push_back(frame);
    }
    // belong folder deleted
    for each (auto name in deletedXmls)
    {
        name.erase(name.length() - 4, 4); // remove extname
        fileManage::frame_t frame = { name, true };
        frames.push_back(frame);
    }
}

fileManage::fileManage(foldSetting f)
    : fold(f)
{
        
}

void fileManage::init()
{
    auto xmls = listFiles(fold.fullAnnotations);
    auto jpgs = listFiles(fold.fullJpegImages);
    auto deletedXmls = listFiles(fold.fullDeleteAnnotations);
    auto deletedJpgs = listFiles(fold.fullDeleteJpegImages);

    if (!isConsistent(xmls, jpgs))
        throw "filenames inside annotations and jpgimages is not same";
    if (!isConsistent(deletedXmls, deletedJpgs))
        throw "filenames inside deleted annotations and jpgimages is not same";

    frames = toFrames(xmls, deletedXmls);
}