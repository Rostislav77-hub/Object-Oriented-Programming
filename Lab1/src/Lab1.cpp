#include <windows.h>
#include "resource.h"
#include "module1.h"
#include "module2.h"

namespace {

const wchar_t kWindowClassName[] = L"Lab1MainWindowClass";
const wchar_t kWindowTitle[]     = L"Лабораторна робота №1 (Ж=15, ІМ-55)";

HINSTANCE g_hInstance = nullptr;
wchar_t g_displayText[256] = L"";

void OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    rc.left += 20; rc.top += 20;
    DrawTextW(hdc, g_displayText, -1, &rc, DT_LEFT | DT_TOP | DT_WORDBREAK);
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_ROBOTA_ROBOTA1: {
            wchar_t buffer[256] = L"";
            if (ShowRobota1Dialog(hWnd, g_hInstance, buffer, 256) == IDOK) {
                lstrcpynW(g_displayText, buffer, 256);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }
        case ID_ROBOTA_ROBOTA2: {
            wchar_t buffer[256] = L"";
            if (ShowRobota2Dialog(hWnd, g_hInstance, buffer, 256) == IDOK) {
                lstrcpynW(g_displayText, buffer, 256);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }
        }
        break;
    case WM_PAINT:
        OnPaint(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

ATOM RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = kWindowClassName;
    wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    return RegisterClassExW(&wc);
}

} 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    g_hInstance = hInstance;
    if (!RegisterMainWindowClass(hInstance)) {
        MessageBoxW(nullptr, L"Не вдалося зареєструвати клас вікна.", L"Помилка", MB_ICONERROR);
        return 1;
    }
    HWND hWnd = CreateWindowExW(0, kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 520, 360, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        MessageBoxW(nullptr, L"Не вдалося створити вікно.", L"Помилка", MB_ICONERROR);
        return 1;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}