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

std::wstring foldSetting::base() const { return base_; }
std::wstring foldSetting::annotations() const { return annotations_; }
std::wstring foldSetting::jpegImages() const { return jpegImages_; }
std::wstring foldSetting::deleteAnnotations() const { return deleteAnnotations_; }
std::wstring foldSetting::deleteJpegImages() const { return deleteJpegImages_; }

std::wstring foldSetting::fullAnnotations() const
{
    return base_ + L"\\" + annotations_;
}

std::wstring foldSetting::fullJpegImages() const
{
    return base_ + L"\\" + jpegImages_;
}

std::wstring foldSetting::fullDeleteAnnotations() const
{
    return base_ + L"\\" + deleteAnnotations_;
}

std::wstring foldSetting::fullDeleteJpegImages() const
{
    return base_ + L"\\" + deleteJpegImages_;
}
