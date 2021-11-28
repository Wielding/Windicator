#include "../include/notificationIcon.h"
#include "../include/messages.h"
#include "../resources/resource.h"

#include <shellapi.h>
#include <strsafe.h>

namespace NotificationIcon {

    NOTIFYICONDATA nid = {};
    HMENU hMenuTrackPopup;
    HMENU hNotifyMenu; // top-level menu

    /// @brief Add the notification icon to the system tray
    /// @param hInst The application instance
    /// @param hWndMain The parent window handle
    /// @param nDesktop The current desktop number
    /// @return result
    HRESULT Add(HINSTANCE hInst, HWND hWndMain, UINT nDesktop)
    {
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWndMain;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

        // Set the notification tip text
        WCHAR szTitle[ARRAYSIZE(nid.szTip)];
        LoadStringW(hInst, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));
        StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), szTitle);

        nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL_START + nDesktop));
        nid.uCallbackMessage = APP_WM_ICON_NOTIFY;

        auto result = Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;

        if (result == E_FAIL) {
            wchar_t err[256];
            memset(err, 0, 256);
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
                    nullptr, GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, nullptr);

            OutputDebugStringW(err);
            MessageBox(hWndMain, err, L"Winidicator Error", MB_ICONERROR);
        }

        return result;
    }

    /// @brief Modify an existing notification icon
    /// @param hInst The application instance
    /// @param hWndMain The parent window handle
    /// @param nDesktop The current desktop number
    /// @return result
    HRESULT Modify(HINSTANCE hInst, HWND hWndMain, UINT nDesktop)
    {
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWndMain;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

        // Set the notification tip text
        WCHAR szTitle[ARRAYSIZE(nid.szTip)];
        LoadStringW(hInst, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));
        StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), szTitle);

        if (nDesktop > 10) {
            nDesktop = 11;
        }

        nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL_START + nDesktop));
        nid.uCallbackMessage = APP_WM_ICON_NOTIFY;

        auto result = Shell_NotifyIcon(NIM_MODIFY, &nid) ? S_OK : E_FAIL;

        return result;
    }

    /// @brief remove the notification icon from the tray
    /// @return HRESULT
    HRESULT Remove()
    {
        return Shell_NotifyIcon(NIM_DELETE, &nid) ? S_OK : E_FAIL;
    }

    /// @brief Display the notification icon context menu
    /// @param hInst Application instance handle
    /// @param hwnd Parent window handle
    /// @param point The location to show the menu
    VOID APIENTRY DisplayContextMenu(HINSTANCE hInst, HWND hwnd, POINT point)
    {
        // Load the menu resource.

        if ((hNotifyMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_NOTIFY_MENU))) ==
                nullptr)
            return;

        // TrackPopupMenu cannot display the menu bar so get
        // a handle to the first shortcut menu.
        hMenuTrackPopup = GetSubMenu(hNotifyMenu, 0);

        // Display the shortcut menu. Track the right mouse
        // button.
        TrackPopupMenu(hMenuTrackPopup,
                TPM_BOTTOMALIGN | TPM_RIGHTALIGN | TPM_RIGHTBUTTON, point.x,
                point.y, 0, hwnd, nullptr);

        // Destroy the menu.
        DestroyMenu(hNotifyMenu);
    }

    /// @brief The notification icon window proc
    /// @return result
    INT_PTR CALLBACK WndProc(HINSTANCE hInst, HWND hWnd,
            UINT message, WPARAM wParam,
            LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (lParam) {
            case WM_LBUTTONDBLCLK:
                SendMessage(hWnd, WM_COMMAND,
                        MAKELPARAM(IDM_NOTIFY_TOGGLE_VISIBLITY, HIWORD(wParam)),
                        lParam);
                break;
            case WM_RBUTTONUP:
                POINT pt; // location of mouse click
                GetCursorPos(&pt);
                DisplayContextMenu(hInst, hWnd, pt);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }
}