#include "../include/notificationIcon.h"
#include "../resources/resource.h"

#include <shellapi.h>
#include <strsafe.h>
#include "../include/messages.h"

namespace NotificationIcon {

	NOTIFYICONDATA nid = {};
	HMENU hMenuTrackPopup;
	HMENU hNotifyMenu; // top-level menu

	HRESULT AddNotificationIcon(HINSTANCE hInst, HWND hWndMain, UINT nDesktop)
	{
		nid.cbSize = sizeof(nid);
		nid.hWnd = hWndMain;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

// It seems this is more trouble than it is worth since it prevents the icon from showing if the
// application is moved after its first run unless you manually delete some registry entries that
// Windows creates on the applications first run.  It makes sense for an application that has an
// installer but not for an app that can be placed anywhere in the system.

//#ifndef _DEBUG
//#ifdef _WIN64
//		nid.uFlags = nid.uFlags | NIF_GUID;
//		nid.guidItem = { 0x5d89e630, 0x3739, 0x4242, { 0x9d, 0xb7, 0xe3, 0xdb, 0x2d, 0xba, 0x2d, 0x3f } };		// {5D89E630-3739-4242-9DB7-E3DB2DBA2D3F}
//#endif
//#endif

		// Set the notification tip text
		WCHAR szTitle[ARRAYSIZE(nid.szTip)];
		LoadStringW(hInst, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));
		StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), szTitle);

		nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL_START + nDesktop));
		nid.uCallbackMessage = APP_WM_ICON_NOTIFY;

		auto result = Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;

		if (result == E_FAIL)
		{
			RemoveNotificationIcon();

			result = Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;

			if (result == E_FAIL)
			{
				wchar_t err[256];
				memset(err, 0, 256);
				FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
					nullptr, GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, nullptr);

				OutputDebugStringW(err);
			}
		}

		return result;
	}

    HRESULT ModifyNotificationIcon(HINSTANCE hInst, HWND hWndMain, UINT nDesktop)
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
	HRESULT RemoveNotificationIcon()
	{
		return Shell_NotifyIcon(NIM_DELETE, &nid) ? S_OK : E_FAIL;
	}

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

	INT_PTR CALLBACK NotificationIconWndProc(HINSTANCE hInst, HWND hWnd,
		UINT message, WPARAM wParam,
		LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (lParam)
		{
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