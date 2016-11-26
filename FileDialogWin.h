#ifndef FileDialogWin_H__
#define FileDialogWin_H__

#include <vector>
#include <string>

std::string w2mb(const wchar_t* w);

// show a folder chooser dialog 
wchar_t* OpenFolderDialog();

// list files in directory
std::vector<std::wstring> listFiles(const wchar_t * dir_name);

// show a folder chooser dialog and list files inside
std::vector<std::wstring> getFileList();

// check a directory is exist or not
bool isDirectoryExist(std::wstring dirPath);

// show message box
void showMessageBox(std::wstring caption, std::wstring context)

#endif

