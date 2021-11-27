#include "../include/framework.h"
#include "../include/MainWindow.h"

#include "../resources/resource.h"

#include <string>


INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
            COINIT_DISABLE_OLE1DDE))) {
        return FALSE;
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

    CoUninitialize();

    return 0;
}
