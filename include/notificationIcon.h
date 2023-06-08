#pragma once

#include "framework.h"

namespace NotificationIcon {

    HRESULT Add(HINSTANCE hInst, HWND hWndMain, UINT nDesktop, UINT iconOffset);

    HRESULT Modify(HINSTANCE hInst, HWND hWndMain, UINT nDesktop, UINT iconOffset);

    HRESULT Remove();

    INT_PTR CALLBACK WndProc(HINSTANCE hInst, HWND hWnd,
            UINT message, WPARAM wParam,
            LPARAM lParam);

    VOID APIENTRY DisplayContextMenu(HINSTANCE hInst, HWND hWnd, POINT point);
}