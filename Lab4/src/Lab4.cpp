#include <windows.h>
#include "resource.h"
#include "my_editor.h"

const wchar_t kClassName[] = L"Lab4MainWindowClass";
const wchar_t kTitle[]     = L"OOP_lab4 (В=15, ІМ-55)";

HINSTANCE g_hInst = nullptr;
MyEditor g_editor; 

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:        g_editor.OnCreate(hWnd, g_hInst); return 0;
    case WM_SIZE:          g_editor.OnSize(hWnd); return 0;
    case WM_NOTIFY:        g_editor.OnNotify(hWnd, wParam, lParam); return 0;
    case WM_INITMENUPOPUP: g_editor.OnInitMenuPopup(hWnd, wParam); return 0;
    case WM_LBUTTONDOWN:   g_editor.OnLBdown(hWnd); return 0;
    case WM_LBUTTONUP:     g_editor.OnLBup(hWnd); return 0;
    case WM_MOUSEMOVE:     g_editor.OnMouseMove(hWnd); return 0;
    case WM_PAINT:         g_editor.OnPaint(hWnd); return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OBJ_POINT:   g_editor.Start(new PointShape(),   ID_OBJ_POINT);   return 0;
        case ID_OBJ_LINE:    g_editor.Start(new LineShape(),    ID_OBJ_LINE);    return 0;
        case ID_OBJ_RECT:    g_editor.Start(new RectShape(),    ID_OBJ_RECT);    return 0;
        case ID_OBJ_ELLIPSE: g_editor.Start(new EllipseShape(), ID_OBJ_ELLIPSE); return 0;
        case ID_OBJ_LINEOO:  g_editor.Start(new LineOOShape(),  ID_OBJ_LINEOO);  return 0;
        case ID_OBJ_CUBE:    g_editor.Start(new CubeShape(),    ID_OBJ_CUBE);    return 0;
        case ID_HELP_ABOUT:
            MessageBoxW(hWnd,
                L"Лабораторна робота №4\nАвтор: Матвіїв Р. С., група ІМ-55 (варіант 15)",
                L"Про програму", MB_OK | MB_ICONINFORMATION);
            return 0;
        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow) {
    g_hInst = hInst;

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor       = LoadCursorW(nullptr, IDC_CROSS);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = kClassName;
    wc.hIconSm       = wc.hIcon;

    if (!RegisterClassExW(&wc)) return 1;

    HWND hWnd = CreateWindowExW(
        0, kClassName, kTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 760, 500, nullptr, nullptr, hInst, nullptr);
    if (!hWnd) return 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}