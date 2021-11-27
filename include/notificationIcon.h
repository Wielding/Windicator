#pragma once

#include "framework.h"

namespace NotificationIcon {

	HRESULT AddNotificationIcon(HINSTANCE hInst, HWND hWndMain, UINT nDesktop);
    HRESULT ModifyNotificationIcon(HINSTANCE hInst, HWND hWndMain, UINT nDesktop);

	HRESULT RemoveNotificationIcon();

	INT_PTR CALLBACK NotificationIconWndProc(HINSTANCE hInst, HWND hWnd,
		UINT message, WPARAM wParam,
		LPARAM lParam);

	VOID APIENTRY DisplayContextMenu(HINSTANCE hInst, HWND hwnd, POINT point);
}