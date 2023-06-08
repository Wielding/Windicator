#include "../resources/resource.h"
#include "../include/MainWindow.h"
#include "../include/messages.h"
#include "../include/notificationIcon.h"

/// @brief Set window class data
/// @param wc Pointer to window class structure
void MainWindow::AmendWindowClass(WNDCLASSEXW* wc)
{
    wc->hIcon = LoadIcon(wc->hInstance, MAKEINTRESOURCE(IDI_SMALL_1));
    wc->hIconSm = LoadIcon(wc->hInstance, MAKEINTRESOURCE(IDI_SMALL_1));
    wc->lpszMenuName = MAKEINTRESOURCE(IDC_MAIN_MENU);
}

/// @brief return the window class name
/// @return window class name
PCWSTR MainWindow::ClassName() const
{
    LoadString(GetModuleHandle(nullptr), IDC_MAIN_MENU,
            const_cast<LPWSTR>(m_szMainWindowClass), MAX_LOAD_STRING);

    return m_szMainWindowClass;
}

/// @brief Handle windows messages
/// @param uMsg message
/// @param wParam WPARAM
/// @param lParam LPARAM
/// @return result
MainWindow::MainWindow(std::shared_ptr<Config> config)
{
    m_config = config;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto* const hWnd = m_hWnd;

    switch (uMsg) {
        case WM_CREATE:

            // Register message to handle re-adding icon after explorer restart.
            m_uMsgTaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));

            NotificationIcon::Add(m_hInstance, m_hWnd, DesktopWatcher::GetCurrentDesktopNumber(nullptr), m_config->iconOffset);

            m_stWatcherData.hWnd = m_hWnd;

            // Start a thread that watches for Virtual Desktop registry changes
            // and posts messages (APP_WM_DESKTOP_CHANGE) to this window when they change.
            m_hDesktopThread = CreateThread(
                    nullptr,
                    0,
                    DesktopWatcher::DesktopWatcherThreadProc,
                    &m_stWatcherData,
                    0,
                    &m_dwThreadId
            );
            break;

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDM_ABOUT:
                    DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDialog::DlgProc);
                    break;

                case IDM_NOTIFY_EXIT:
                case IDM_EXIT: {
                    std::lock_guard lock(m_stWatcherData.lock);
                    m_stWatcherData.keepGoing = FALSE;
                }
#ifdef _TIDY_TIMEOUT
                    WaitForSingleObject(m_hDesktopThread, 2000);
#endif
                    NotificationIcon::Remove();
                    DestroyWindow(hWnd);
                    break;
                case IDM_NOTIFY_TOGGLE_VISIBILITY:
                    m_isVisible = !m_isVisible;
                    Show(m_isVisible ? SW_SHOW : SW_HIDE);
                    break;
                default:
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }
            break;

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)));
            EndPaint(hWnd, &ps);
        }
            return 0;

        case APP_WM_ICON_NOTIFY:
            return NotificationIcon::WndProc(GetModuleHandle(nullptr), hWnd, uMsg, wParam, lParam);

        case APP_WM_DESKTOP_CHANGE: {
            auto id = LOWORD(lParam);

            NotificationIcon::Modify(m_hInstance, m_hWnd, id, m_config->iconOffset);
        }

            return 0;

        default:
            if (uMsg == m_uMsgTaskbarCreated) {
                // Put the icon back if explorer has been restarted after a termination or crash
                NotificationIcon::Add(m_hInstance, m_hWnd, DesktopWatcher::GetCurrentDesktopNumber(nullptr), m_config->iconOffset);
            }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

