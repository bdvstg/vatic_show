#include <algorithm>

#include "FileDialogWin.h"
#include "fileManage.h"

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
        jpgname.erase(jpgname.length() - 4, 4);
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

    return frames;
}

fileManage::fileManage(foldSetting f)
    : fold(f)
{
        
}

void fileManage::init()
{
    auto xmls = listFiles(fold.fullAnnotations().c_str());
    auto jpgs = listFiles(fold.fullJpegImages().c_str());
    auto deletedXmls = listFiles(fold.fullDeleteAnnotations().c_str());
    auto deletedJpgs = listFiles(fold.fullDeleteJpegImages().c_str());

    if (!isConsistent(xmls, jpgs))
        throw "filenames inside annotations and jpgimages is not same";
    if (!isConsistent(deletedXmls, deletedJpgs))
        throw "filenames inside deleted annotations and jpgimages is not same";

    frames = toFrames(xmls, deletedXmls);

    std::sort(frames.begin(), frames.end(),
        [](const frame_t & a, const frame_t & b) -> bool
        {
            auto la = wcstol(a.baseName.c_str(), nullptr, 10);
            auto lb = wcstol(b.baseName.c_str(), nullptr, 10);
            return la < lb;
        });
}

std::wstring fileManage::getImageName(int idx) const
{
    std::wstring extname = L".jpg";
    std::wstring fullname;
    const auto & frame = frames[idx];
    if (frame.isDeleted)
        fullname = fold.fullDeleteJpegImages() + L"\\" + frame.baseName + extname;
    else
        fullname = fold.fullJpegImages() + L"\\" + frame.baseName + extname;
    return fullname;
}

std::wstring fileManage::getXmlName(int idx) const
{
    std::wstring extname = L".xml";
    std::wstring fullname;
    const auto & frame = frames[idx];
    if (frame.isDeleted)
        fullname = fold.fullDeleteAnnotations() + L"\\" + frame.baseName + extname;
    else
        fullname = fold.fullAnnotations() + L"\\" + frame.baseName + extname;
    return fullname;
}

bool fileManage::isDeleted(int idx) const
{
    return frames[idx].isDeleted;
}