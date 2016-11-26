#ifndef foldSetting_H__
#define foldSetting_H__

#include <string>

class foldSetting
{
public:
    foldSetting(
        std::wstring basePath,
        std::wstring annotationsName,
        std::wstring jpegImagesName,
        std::wstring deleteAnnotationsName,
        std::wstring deleteJpegImagesName);

    
    std::wstring base();
    std::wstring annotations();
    std::wstring jpegImages();
    std::wstring deleteAnnotations();
    std::wstring deleteJpegImages();

    std::wstring fullAnnotations();
    std::wstring fullJpegImages();
    std::wstring fullDeleteAnnotations();
    std::wstring fullDeleteJpegImages();

private:
    std::wstring base_;
    std::wstring annotations_;
    std::wstring jpegImages_;
    std::wstring deleteAnnotations_;
    std::wstring deleteJpegImages_;
};

#endif
