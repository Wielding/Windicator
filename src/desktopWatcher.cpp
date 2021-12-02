#include "../include/framework.h"
#include "../include/desktopWatcher.h"
#include "../include/messages.h"

#include <vector>

namespace DesktopWatcher {

    /// @brief Display a MessageBox with the translated error from errorCode
    /// @param hParent Parent window handle
    /// @param errorCode Error code to translate
    VOID ShowErrorMessageBox(HWND hParent, DWORD errorCode)
    {
        wchar_t errorMessage[256] = {};
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
                nullptr, errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMessage, sizeof(errorMessage), nullptr);

        OutputDebugString(errorMessage);
        MessageBox(hParent, errorMessage, L"Windicator Error", MB_ICONERROR);
    }

    /// @brief Get the current desktop number
    /// @param hKey handle to key or nullptr to allocate local handle
    /// @return Desktop Number
    UINT GetCurrentDesktopNumber(HKEY hKey)
    {
        BYTE buffer[4096] = {};
        PVOID pvData = buffer;
        LPDWORD pdwType = nullptr;
        DWORD size = sizeof(buffer);
        LONG status{};
        BOOL localHandle = (hKey == nullptr);

        if (localHandle) {

            REGSAM samDesired = KEY_READ | KEY_NOTIFY;

            status = RegOpenKeyEx(
                    HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
                    0,
                    samDesired,
                    &hKey
            );
        }

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

        if (ERROR_SUCCESS != status) {
            return 0;
        }

        std::vector<GUID> desktops;

        // create list of desktop GUIDS using the binary data retrieved from the registry
        for (size_t i = 0; i < size / 16; i++) {

            GUID desktopId;
            memcpy(&desktopId, &static_cast<BYTE*>(pvData)[i * 16], 16);

            desktops.push_back(desktopId);
        }

        size = sizeof(buffer);

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
        UINT idx = 1;
        UINT desktopNumber = 1;
        for (auto& desktopId: desktops) {
            if (desktopId == currentDesktopId) {
                desktopNumber = idx;
            }
            idx++;
        }

        if (localHandle) {
            RegCloseKey(hKey);
        }

        return desktopNumber;
    }

    /// @brief Thread to watch for registry changes related to Virtual Desktops
    /// @param lParam Pointer to a DesktopWatcher structure.
    /// @return result
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

            // This will probably only trigger if _TIDY_TIMEOUT is defined since we are likely
            // waiting for a registry change event when the parent process sets keepGoing to FALSE
            // and terminates.
            {
                std::lock_guard lock(pData->lock);
                keepGoing = pData->keepGoing;
                if (!keepGoing) {
                    continue;
                }
            }

            // Create an event to wait on for the registry changes
            auto hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

            if (hEvent == nullptr) {
                ShowErrorMessageBox(pData->hWnd, GetLastError());
                keepGoing = FALSE;
                continue;
            }

            // Watch the registry key for a change of buffer.
            if (!first) {
                status = RegNotifyChangeKeyValue(hKey,
                        TRUE,
                        dwFilter,
                        hEvent,
                        TRUE);

                if (status != ERROR_SUCCESS) {
                    ShowErrorMessageBox(pData->hWnd, GetLastError());
                    keepGoing = FALSE;
                    continue;
                }

                // This is an optional build definition to use a time-out to make sure we can catch the program exit
                // so the registry handle can be closed.  It is probably not needed since this app will likely
                // run the duration of your session so leaking a single registry handle is not an issue.  The system
                // would eventually clean it up anyway.  If you want to use it uncomment the _TIDY_TIMEOUT definition
                // in the CMakeLists.txt before you build.  It will create a tiny increase in CPU usage since the
                // thread will loop instead of an infinite wait event for a registry change.
#ifdef _TIDY_TIMEOUT
                auto timeout = 500;
#else
                auto timeout = INFINITE;
#endif
                auto result = WaitForSingleObject(hEvent, timeout);

                if (result == WAIT_FAILED) {
                    ShowErrorMessageBox(pData->hWnd, GetLastError());
                    keepGoing = FALSE;
                    continue;
                }

                if (result == WAIT_TIMEOUT) {
                    continue;
                }
            }

            first = FALSE;

            auto desktopNumber = GetCurrentDesktopNumber(hKey);
            PostMessage(pData->hWnd, APP_WM_DESKTOP_CHANGE, 0, MAKELPARAM(desktopNumber, 0));
        }


        // We most likely will not hit this unless _TIDY_TIMEOUT is defined at compile time or
        // an error condition occurred.
        RegCloseKey(hKey);

        return TRUE;
    }

}