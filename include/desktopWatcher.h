#pragma once

#include <shared_mutex>

namespace DesktopWatcher {

    struct DesktopWatcherData {
        HWND hWnd{};
        BOOL keepGoing{TRUE};
        std::shared_mutex lock{};
    };

    VOID ShowErrorMessageBox(HWND hParent, DWORD errorCode);

    UINT GetCurrentDesktopNumber(HKEY hKey);

    DWORD WINAPI DesktopWatcherThreadProc(LPVOID lParam);
}