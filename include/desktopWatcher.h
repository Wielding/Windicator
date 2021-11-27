#pragma once

#include <shared_mutex>

namespace DesktopWatcher {

    struct DesktopWatcherData {
        HWND hWnd{};
        BOOL keepGoing{TRUE};
        std::shared_mutex lock{};
    };


    DWORD WINAPI DesktopWatcherThreadProc(LPVOID lParam);

}