#pragma once

#include <string>

#include "framework.h"


namespace AboutDialog
{
	typedef struct tagVersionInfo {
		std::wstring versionText;
	} VERSION_INFO;

	INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

}

