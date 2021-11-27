#pragma once

namespace DesktopWatcher {

    struct DesktopWatcherData {
        HWND hWnd;
        BYTE desktopNumber;
    };


    DWORD WINAPI DesktopWatcherThreadProc(LPVOID lParam);

}