#ifndef FileDialogWin_H__
#define FileDialogWin_H__

#include <vector>
#include <string>

using filenames_t = std::vector<std::wstring>;

std::string w2mb(const wchar_t* w);

// show a folder chooser dialog 
wchar_t* OpenFolderDialog();

// list files in directory
filenames_t listFiles(const wchar_t * dir_name);

// show a folder chooser dialog and list files inside
filenames_t getFileList();

// check a directory is exist or not
bool isDirectoryExist(std::wstring dirPath);

// show message box
void showMessageBox(std::wstring caption, std::wstring context);

// create a directory, if success, return true
bool createFolder(std::wstring path);

// move file (or directory)
bool moveFile(std::wstring src, std::wstring dest);

#endif

