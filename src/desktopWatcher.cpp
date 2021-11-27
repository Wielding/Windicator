#include "../include/framework.h"
#include "../include/desktopWatcher.h"
#include "../include/messages.h"
#include <vector>
#include <sstream>

namespace DesktopWatcher {


    DWORD DesktopWatcherThreadProc(LPVOID lParam)
    {
        auto* const pData = static_cast<DesktopWatcherData*>(lParam);

        DWORD  dwFilter = REG_NOTIFY_CHANGE_NAME |
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

        while (true) {
            // Create an event.
            auto hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

            if (hEvent == nullptr) {
                return 8;
            }

            // Watch the registry key for a change of value.
            if (!first) {
                status = RegNotifyChangeKeyValue(hKey,
                        TRUE,
                        dwFilter,
                        hEvent,
                        TRUE);

                if (status != ERROR_SUCCESS) {
                    // TODO: handle error
                    return 8;
                }


                if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED) {
                    // TODO: handle error
                }
            }

            first = FALSE;

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

            if (ERROR_SUCCESS != status) {
                return 8;
            }

            std::vector<GUID> desktops;

            // create GUIDS from the binary data
            for (size_t i = 0; i < size / 16; i++) {

                GUID g;
                memcpy(&g, &static_cast<BYTE*>(pvData)[i * 16], 16);

                desktops.push_back(g);
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

            GUID c;
            memcpy(&c, &static_cast<BYTE*>(pvData)[0], 16);

            // check which GUID in desktops matches the current desktop.
            auto idx = 1;
            for (auto& i : desktops) {
                if (i == c) {
                    std::wstringstream ss;
                    ss << L"On desktop #" << idx << std::endl;
                    PostMessage(pData->hWnd, APP_WM_DESKTOP_CHANGE, 0, MAKELPARAM(idx, 0));
                    OutputDebugString(ss.str().c_str());
                    continue;
                }
                idx++;
            }

        }

        RegCloseKey(hKey);




        return TRUE;
    }
}