#include "foldSetting.h"

foldSetting::foldSetting(
    std::wstring basePath,
    std::wstring annotationsName,
    std::wstring jpegImagesName,
    std::wstring deleteAnnotationsName,
    std::wstring deleteJpegImagesName)
    : base_(basePath)
    , annotations_(annotationsName)
    , jpegImages_(jpegImagesName)
    , deleteAnnotations_(deleteAnnotationsName)
    , deleteJpegImages_(deleteJpegImagesName)
{

}

std::wstring foldSetting::base() { return base_; }
std::wstring foldSetting::annotations() { return annotations_; }
std::wstring foldSetting::jpegImages() { return jpegImages_; }
std::wstring foldSetting::deleteAnnotations() { return deleteAnnotations_; }
std::wstring foldSetting::deleteJpegImages() { return deleteJpegImages_; }

std::wstring foldSetting::fullAnnotations()
{
    return base_ + L"\\" + annotations_;
}

std::wstring foldSetting::fullJpegImages()
{
    return base_ + L"\\" + jpegImages_;
}

std::wstring foldSetting::fullDeleteAnnotations()
{
    return base_ + L"\\" + deleteAnnotations_;
}

std::wstring foldSetting::fullDeleteJpegImages()
{
    return base_ + L"\\" + deleteJpegImages_;
}
