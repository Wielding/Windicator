#pragma once

#include "framework.h"

#include <string>

namespace AboutDialog {

    struct VersionInfo {
        std::wstring versionText;
    };

    INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
}

