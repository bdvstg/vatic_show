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

    
    std::wstring base() const;
    std::wstring annotations() const;
    std::wstring jpegImages() const;
    std::wstring deleteAnnotations() const;
    std::wstring deleteJpegImages() const;

    std::wstring fullAnnotations() const;
    std::wstring fullJpegImages() const;
    std::wstring fullDeleteAnnotations() const;
    std::wstring fullDeleteJpegImages() const;

private:
    std::wstring base_;
    std::wstring annotations_;
    std::wstring jpegImages_;
    std::wstring deleteAnnotations_;
    std::wstring deleteJpegImages_;
};

#endif
