#include "../include/aboutdialog.h"
#include "../resources/resource.h"

#include <shellapi.h>
#include <string>
#include <CommCtrl.h>

namespace AboutDialog {
    // This is a global for others to access (e.g. version check logic)
    VersionInfo gStVersionInfo{};

    /// @brief AboutDialog window proc
    INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (message) {
            case WM_INITDIALOG: {
                // extract the version information from the resources
                wchar_t moduleFileName[MAX_PATH];
                GetModuleFileNameW(GetModuleHandle(nullptr), moduleFileName, MAX_PATH);
                DWORD ptr;

                const auto versionInfoSize = GetFileVersionInfoSize(moduleFileName, &ptr);
                auto* const versionInfo = new char[versionInfoSize];

                GetFileVersionInfo(moduleFileName, 0, versionInfoSize, versionInfo);

                UINT pBlockSize;
                LPVOID pVersionBlock = nullptr;

                // the code page is fixed to (0409)US English, (04b0)Unicode.  Enhance that someday.
                VerQueryValue(versionInfo, L"\\StringFileInfo\\040904b0\\FileVersion", &pVersionBlock, &pBlockSize);
                gStVersionInfo.versionText = static_cast<const wchar_t*>(pVersionBlock);

                delete[] versionInfo;

                HWND staticTextHandle = GetDlgItem(hDlg, IDC_VERSION_TEXT);
                std::wstring versionText = L"Version " + gStVersionInfo.versionText;
                SetWindowText(staticTextHandle, versionText.c_str());

                return TRUE;
            }

            case WM_COMMAND:
                if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                }
                break;

            case WM_NOTIFY:
            {
                switch (reinterpret_cast<LPNMHDR>(lParam)->code)
                {
                    case NM_CLICK:
                    case NM_RETURN:
                        auto* pNmLink = reinterpret_cast<PNMLINK>(lParam);
                        std::wstring url = pNmLink->item.szUrl;

                        ShellExecute(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOW);
                        break;
                }
            }

            default:;
        }
        return FALSE;
    }
}

