#include "../resources/resource.h"
#include "../include/MainWindow.h"
#include "../include/messages.h"
#include "../include/notificationIcon.h"
#include "../include/desktopWatcher.h"

/// @brief set window class data
/// @param wc pointer to window class structure
void MainWindow::AmendWindowClass(WNDCLASSEXW* wc)
{
	wc->hIcon = LoadIcon(wc->hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wc->hIconSm = LoadIcon(wc->hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wc->lpszMenuName = MAKEINTRESOURCEW(IDC_MAIN_MENU);
}

/// @brief return the window class name
/// @return window class name
PCWSTR MainWindow::ClassName() const
{
	LoadStringW(GetModuleHandle(nullptr), IDC_MAIN_MENU,
			const_cast<LPWSTR>(m_szMainWindowClass), MAX_LOAD_STRING);

	return m_szMainWindowClass;
}

/// @brief Handle windows messages
/// @param uMsg message
/// @param wParam WPARAM
/// @param lParam LPARAM
/// @return LRESULT
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto* const hWnd = m_hWnd;

	switch (uMsg)
	{
	case WM_CREATE:
		NotificationIcon::AddNotificationIcon(m_hInstance, m_hWnd, 1);

        watcher_data.hWnd = m_hWnd;


        m_hDesktopThread = CreateThread(
                nullptr,
                0,
                DesktopWatcher::DesktopWatcherThreadProc,
                &watcher_data,
                0,
                &m_dwThreadId
                );
		break;

	case APP_WM_ICON_NOTIFY:
		return NotificationIcon::NotificationIconWndProc(GetModuleHandle(nullptr), hWnd, uMsg, wParam, lParam);

	case WM_COMMAND:
	{
		switch (const int wmId = LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDialog::DlgProc);
			break;

		case IDM_NOTIFY_EXIT:
		case IDM_EXIT:
			NotificationIcon::RemoveNotificationIcon();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
		break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)));
		EndPaint(hWnd, &ps);
	}
		return 0;

    case APP_WM_DESKTOP_CHANGE: {
        auto id = LOWORD(lParam);

        NotificationIcon::ModifyNotificationIcon(m_hInstance, m_hWnd, id);
    }

        return 0;

	default:;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void MainWindow::Show(int nShowCmd) const
{
	ShowWindow(m_hWnd, nShowCmd);
}


