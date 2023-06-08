#include "../include/framework.h"
#include "../include/MainWindow.h"
#include "../resources/resource.h"
#include "../include/Config.h"

#include <string>
#include <memory>

/// @brief Application entry point
/// @return result
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    std::wstring mutexName = L"Windicator_Instance";

    HANDLE mutex = CreateMutex(nullptr, TRUE, mutexName.c_str());

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(nullptr, L"An instance of Windicator is already running", L"Windicator", MB_ICONEXCLAMATION);
        return 8;
    }

    wchar_t szWindowName[MAX_LOAD_STRING];

    LoadString(hInstance, IDS_APP_TITLE, szWindowName, MAX_LOAD_STRING);

    std::shared_ptr<Config> config = std::make_shared<Config>();

    std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>(config);

    if (!mainWindow->Create(szWindowName, WS_OVERLAPPED)) {
        return 0;
    }

    mainWindow->Show(SW_HIDE);

    auto* const hAccelerators =
            LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAIN_MENU));

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelerators, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ReleaseMutex(mutex);
    CloseHandle(mutex);

    return 0;
}
