#include "../include/framework.h"
#include "../include/MainWindow.h"
#include "../resources/resource.h"

#include <string>

/// @brief Application entry point
/// @return result
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{

    CoInitialize(nullptr);
    std::wstring mutexName = L"Windicator_Instance";

    HANDLE mutex = CreateMutex(nullptr, TRUE, mutexName.c_str());

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(nullptr, L"An instance of Windicator is already running", L"Windicator", MB_ICONEXCLAMATION);
        return 8;
    }

    wchar_t szWindowName[MAX_LOAD_STRING];

    LoadStringW(hInstance, IDS_APP_TITLE, szWindowName, MAX_LOAD_STRING);

    MainWindow mainWindow;

    if (!mainWindow.Create(szWindowName, WS_OVERLAPPED)) {
        return 0;
    }

    mainWindow.Show(SW_HIDE);

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

    CoUninitialize();

    return 0;
}
