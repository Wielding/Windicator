#include "../include/notificationIcon.h"
#include "../include/messages.h"
#include "../resources/resource.h"

#include <shellapi.h>
#include <strsafe.h>

namespace NotificationIcon {

    NOTIFYICONDATA nid = {};
    HMENU hMenuTrackPopup;
    HMENU hNotifyMenu;

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

        WCHAR szTitle[ARRAYSIZE(nid.szTip)];
        LoadStringW(hInst, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));
        StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), szTitle);

        nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BLUE_SMALL_START + nDesktop));
        nid.uCallbackMessage = APP_WM_ICON_NOTIFY;

        auto result = Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;

        if (result == E_FAIL) {
            wchar_t err[256];
            memset(err, 0, 256);
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
                    nullptr, GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, nullptr);

            OutputDebugString(err);
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
        nid.uFlags = NIF_ICON;

        if (nDesktop > 10) {
            nDesktop = 11;
        }

        nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BLUE_SMALL_START + nDesktop));

        auto result = Shell_NotifyIcon(NIM_MODIFY, &nid) ? S_OK : E_FAIL;

        return result;
    }

    /// @brief remove the notification icon from the tray
    /// @return result from Shell_NotifyIcon
    HRESULT Remove()
    {
        return Shell_NotifyIcon(NIM_DELETE, &nid) ? S_OK : E_FAIL;
    }

    /// @brief Display the notification icon context menu
    /// @param hInst Application instance handle
    /// @param hWnd Parent window handle
    /// @param point The location to show the menu
    VOID APIENTRY DisplayContextMenu(HINSTANCE hInst, HWND hWnd, POINT point)
    {
        if ((hNotifyMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_NOTIFY_MENU))) ==
                nullptr)
            return;

        // TrackPopupMenu cannot display the menu bar so get
        // a handle to the first shortcut menu.
        hMenuTrackPopup = GetSubMenu(hNotifyMenu, 0);

        SetForegroundWindow(hWnd);

        // Display the shortcut menu. Track the right mouse
        // button.
        TrackPopupMenu(hMenuTrackPopup,
                TPM_BOTTOMALIGN | TPM_RIGHTALIGN | TPM_RIGHTBUTTON, point.x,
                point.y, 0, hWnd, nullptr);

        DestroyMenu(hNotifyMenu);
    }

    /// @brief The notification icon window proc
    /// @return result
    INT_PTR CALLBACK WndProc(HINSTANCE hInst, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (lParam) {
            case WM_LBUTTONDBLCLK:
// Disabled for now.  Might be re-enabled if configuration options are added.
//                SendMessage(hWnd, WM_COMMAND,
//                        MAKELPARAM(IDM_NOTIFY_TOGGLE_VISIBILITY, HIWORD(wParam)),
//                        lParam);
                break;
            case WM_RBUTTONUP:
                POINT pt;
                GetCursorPos(&pt);
                DisplayContextMenu(hInst, hWnd, pt);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }
}