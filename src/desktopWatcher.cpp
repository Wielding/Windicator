#include "../include/framework.h"
#include "../include/desktopWatcher.h"
#include "../include/messages.h"
#include <vector>

namespace DesktopWatcher {

    VOID ShowErrorMessageBox(HWND hParent, DWORD errorCode)
    {
        wchar_t errorMessage[256] = {};
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
                nullptr, errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMessage, sizeof(errorMessage), nullptr);

        OutputDebugString(errorMessage);
        MessageBox(hParent, errorMessage, L"Winidicator Error", MB_ICONERROR);

    }

    DWORD DesktopWatcherThreadProc(LPVOID lParam)
    {
        auto* const pData = static_cast<DesktopWatcherData*>(lParam);

        DWORD dwFilter = REG_NOTIFY_CHANGE_NAME |
                REG_NOTIFY_CHANGE_ATTRIBUTES |
                REG_NOTIFY_CHANGE_LAST_SET |
                REG_NOTIFY_CHANGE_SECURITY;

        BOOL first = TRUE;

        REGSAM samDesired = KEY_READ | KEY_NOTIFY;
        HKEY hKey;

        auto status = RegOpenKeyEx(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
                0,
                samDesired,
                &hKey
        );

        BOOL keepGoing = TRUE;

        while (keepGoing) {

            // This will most probably only trigger if _TIDY_TIMEOUT is defined since we are likely
            // waiting for a registry change event when the process terminates.
            {
                std::lock_guard lock(pData->lock);
                keepGoing = pData->keepGoing;
                if (!keepGoing) {
                    continue;
                }
            }

            // Create an event to wait on for the registry changes
            auto hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

            if (hEvent==nullptr) {
                ShowErrorMessageBox(pData->hWnd, GetLastError());
                return 8;
            }

            // Watch the registry key for a change of value.
            if (!first) {
                status = RegNotifyChangeKeyValue(hKey,
                        TRUE,
                        dwFilter,
                        hEvent,
                        TRUE);

                if (status!=ERROR_SUCCESS) {
                    ShowErrorMessageBox(pData->hWnd, GetLastError());
                    return 8;
                }

                // This is an optional build definition to use a time-out to make sure we can catch the program exit
                // so the registry handle can be closed.  It is probably not needed since this app will likely
                // run the duration of your session so leaking a single registry handle is not an issue.  The system
                // would eventually clean it up anyway.  If you want to use it uncomment the _TIDY_TIMEOUT definition
                // in the CMakeLists.txt.  It will create a tiny increase in CPU usage since the thread will loop
                // instead of an infinite wait event for a registry change.
#ifdef _TIDY_TIMEOUT
                auto timeout = 500;
#else
                auto timeout = INFINITE;
#endif
                auto result = WaitForSingleObject(hEvent, timeout);

                if (result==WAIT_FAILED) {
                    ShowErrorMessageBox(pData->hWnd, GetLastError());
                    return 8;
                }

                if (result==WAIT_TIMEOUT) {
                    continue;
                }
            }

            first = FALSE;

            // this buffer will allow for 256 virtual desktops.  That should satisfy just about everyone.
            // We only have indicators for 10 so there you go.
            BYTE value[4096] = {};
            PVOID pvData = value;
            LPDWORD pdwType = nullptr;
            DWORD size = sizeof(value);

            // Get the current desktop ids
            status = RegGetValue(
                    hKey,
                    nullptr,
                    L"VirtualDesktopIds",
                    RRF_RT_REG_BINARY,
                    pdwType,
                    pvData,
                    &size
            );

            if (ERROR_SUCCESS!=status) {
                ShowErrorMessageBox(pData->hWnd, GetLastError());
                return 8;
            }

            std::vector<GUID> desktops;

            // create GUIDS from the binary data
            for (size_t i = 0; i<size/16; i++) {

                GUID desktopId;
                memcpy(&desktopId, &static_cast<BYTE*>(pvData)[i*16], 16);

                desktops.push_back(desktopId);
            }

            size = sizeof(value);

            // get the current virtual desktop id
            status = RegGetValue(
                    hKey,
                    nullptr,
                    L"CurrentVirtualDesktop",
                    RRF_RT_REG_BINARY,
                    pdwType,
                    pvData,
                    &size
            );

            GUID currentDesktopId;
            memcpy(&currentDesktopId, &static_cast<BYTE*>(pvData)[0], 16);

            // check which GUID in desktops matches the current desktop and send
            // a message to the main window proc containing the desktop number in the
            // low word of the LPARAM.
            auto idx = 1;
            for (auto& desktopId: desktops) {
                if (desktopId==currentDesktopId) {
                    PostMessage(pData->hWnd, APP_WM_DESKTOP_CHANGE, 0, MAKELPARAM(idx, 0));
                    continue;
                }
                idx++;
            }
        }

        // We most likely will not hit this unless _TIDY_TIMEOUT is defined at compile time.
        RegCloseKey(hKey);

        return TRUE;
    }


}