#ifndef fileManage_H__
#define fileManage_H__

#include <string>
#include <vector>

#include "FileDialogWin.h"
#include "foldSetting.h"

class fileManage
{
public:
    using frame_t =
    struct {
        std::wstring baseName; // filename without extname
        bool isDeleted; // which folder belong, deleted or not
    };
    using frames_t = std::vector<frame_t>;

    fileManage(foldSetting f);
    void init();

    std::wstring getImageName(int idx) const;
    std::wstring getXmlName(int idx) const;
    bool isDeleted(int idx) const;

private:
    foldSetting fold;
    frames_t frames; // more compact struct to hold all filenames
};

#endif
