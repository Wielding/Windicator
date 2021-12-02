#pragma once
#pragma once

constexpr int MAX_LOAD_STRING = 100;

/// @brief This is the template class for the main window to allow encapsulating the window logic in a class for
// better organization
/// @tparam DerivedType
template<class DerivedType>
class BaseWindow {
public:
    HMODULE m_hInstance{};

    BaseWindow()
            :m_hWnd(nullptr) { }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DerivedType* pThis;

        if (uMsg == WM_NCCREATE) {
            auto* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = static_cast<DerivedType*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

            pThis->m_hWnd = hWnd;
        }
        else {
            pThis = reinterpret_cast<DerivedType*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    BOOL Create(
            PCWSTR lpWindowName,
            DWORD dwStyle,
            DWORD dwExStyle = 0,
            int x = CW_USEDEFAULT,
            int y = CW_USEDEFAULT,
            int nWidth = CW_USEDEFAULT,
            int nHeight = CW_USEDEFAULT,
            HWND hWndParent = nullptr,
            HMENU hMenu = nullptr
    )
    {
        m_hInstance = GetModuleHandle(nullptr);
        WNDCLASSEXW wc{};

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = DerivedType::WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = ClassName();
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        AmendWindowClass(&wc);

        RegisterClassEx(&wc);

        m_hWnd = CreateWindowEx(
                dwExStyle, wc.lpszClassName, lpWindowName, dwStyle, x, y,
                nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(nullptr), this
        );

        return m_hWnd ? TRUE : FALSE;
    }

    void Show(int nShowCmd) const
    {
        ShowWindow(m_hWnd, nShowCmd);
    }

protected:
    HWND m_hWnd;

    ~BaseWindow() = default;

    [[nodiscard]] virtual PCWSTR ClassName() const = 0;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    // Override AmendWindowClass in your derived class to add app specific
    // resources to the window class (e.g. icons and menu).
    virtual void AmendWindowClass(WNDCLASSEXW* wc)
    {
    }
};
