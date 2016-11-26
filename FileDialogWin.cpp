#include <vector>
#include <string>
#include <mbstring.h>
#include <windows.h>
#include <shobjidl.h> 
#include <strsafe.h>
#pragma comment(lib, "User32.lib")


std::string w2mb(const wchar_t* w)
{
    size_t origsize = wcslen(w) + 1;
    size_t convertedChars = 0;


    const size_t newsize = origsize * 2;


    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;


    char *nstring = new char[newsize + strConcatsize];


    // Put a copy of the converted string into nstring
    wcstombs_s(&convertedChars, nstring, newsize, w, _TRUNCATE);
    // append the type of string to the new string.
    //_mbscat_s((unsigned char*)nstring, newsize + strConcatsize, (unsigned char*)strConcat);
    // Display the result.
    //printf("%s\n",nstring);
    return std::string(nstring);
}


wchar_t* OpenFolderDialog()
{
    PWSTR path = nullptr;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileDialog *pfd;
        if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
        {
            DWORD dwOptions;
            if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
            {
                pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
            }
            if (SUCCEEDED(pfd->Show(NULL)))
            {
                IShellItem *psi;
                if (SUCCEEDED(pfd->GetResult(&psi)))
                {
                    if (!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path)))
                    {
                        MessageBox(NULL, "GetIDListName() failed", NULL, NULL);
                    }
                    psi->Release();
                }
            }
            pfd->Release();
        }
        CoUninitialize();
    }
    //std::string cpath = w2mb(path);
    return path;
}




std::vector<std::wstring> listFiles(const wchar_t * dir_name)
{
    WIN32_FIND_DATAW ffd;
    LARGE_INTEGER filesize;
    wchar_t szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;


    StringCchCopyW(szDir, MAX_PATH, dir_name);
    StringCchCatW(szDir, MAX_PATH, L"\\*");


    // Find the first file in the directory.


    hFind = FindFirstFileW(szDir, &ffd);


    if (INVALID_HANDLE_VALUE == hFind)
    {
        return std::vector<std::wstring>();
    }


    // List all the files in the directory with some info about them.
    std::vector<std::wstring> list;
    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            //printf("%s\n", ffd.cFileName);
        }
        else
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            //printf("%s (%ld)\n", ffd.cFileName, filesize.QuadPart);
            list.push_back(ffd.cFileName);
        }
    } while (FindNextFileW(hFind, &ffd) != 0);
    FindClose(hFind);
    return list;
}


std::vector<std::wstring> getFileList()
{
    wchar_t* folder = OpenFolderDialog();
    
    std::vector<std::wstring> list = listFiles(folder);
    


    return list;
}


bool isDirectoryExist(std::wstring dirPath)
{
    DWORD dwAttrib = GetFileAttributesW(dirPath.c_str());

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void showMessageBox(std::wstring caption, std::wstring context)
{
    MessageBoxW(nullptr, context.c_str(), caption.c_str(), 0);
}

bool createFolder(std::wstring path)
{
    bool success = CreateDirectoryW(path.c_str(), nullptr);
}