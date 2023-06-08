#pragma once

#include "aboutdialog.h"
#include "BaseWindow.h"
#include "desktopWatcher.h"
#include "Config.h"

#include <memory>

class MainWindow : public BaseWindow<MainWindow> {
protected:
    void AmendWindowClass(WNDCLASSEXW* wc) override;

    [[nodiscard]] PCWSTR ClassName() const override;

    DesktopWatcher::DesktopWatcherData m_stWatcherData{};

    std::shared_ptr<Config> m_config;

public:
    MainWindow(std::shared_ptr<Config> config);
    virtual ~MainWindow() = default;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    WCHAR m_szMainWindowClass[MAX_LOAD_STRING]{0};
    DWORD m_dwThreadId{};
    HANDLE m_hDesktopThread{};
    BOOL m_isVisible{FALSE};
    UINT m_uMsgTaskbarCreated{};
};
