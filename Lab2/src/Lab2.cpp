#include <windows.h>
#include "resource.h"
#include "shape_editor.h"

namespace {

const wchar_t kWindowClassName[] = L"Lab2MainWindowClass";
const wchar_t kWindowTitle[]     = L"OOP_lab2 (Ж=15, ІМ-55)";

ShapeObjectsEditor g_editor;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
        g_editor.OnLBdown(hWnd);
        return 0;

    case WM_LBUTTONUP:
        g_editor.OnLBup(hWnd);
        return 0;

    case WM_MOUSEMOVE:
        g_editor.OnMouseMove(hWnd);
        return 0;

    case WM_PAINT:
        g_editor.OnPaint(hWnd);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OBJ_POINT:
            g_editor.StartPointEditor(hWnd);
            return 0;
        case ID_OBJ_LINE:
            g_editor.StartLineEditor(hWnd);
            return 0;
        case ID_OBJ_RECT:
            g_editor.StartRectEditor(hWnd);
            return 0;
        case ID_OBJ_ELLIPSE:
            g_editor.StartEllipseEditor(hWnd);
            return 0;
        case ID_HELP_ABOUT:
            MessageBoxW(hWnd,
                L"Лабораторна робота №2\nРозробка графічного редактора об'єктів\nАвтор: Матвіїв Р. С., група ІМ-55 (Ж=15)",
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
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

ATOM RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor       = LoadCursorW(nullptr, IDC_CROSS);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = kWindowClassName;
    wc.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);
    return RegisterClassExW(&wc);
}

} 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    if (!RegisterMainWindowClass(hInstance)) {
        MessageBoxW(nullptr, L"Не вдалося зареєструвати клас вікна.", L"Помилка", MB_ICONERROR);
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        0, kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 720, 480,
        nullptr, nullptr, hInstance, nullptr);

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