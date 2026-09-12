#include <windows.h>
#include "resource.h"
#include "module1.h"
#include "module2.h"

const char g_szClassName[] = "myWindowClass";
char g_DisplayText[256] = "Оберіть пункт меню 'Робота'";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case ID_WORK1: {
                    if (ShowDialog1(hwnd, g_DisplayText, sizeof(g_DisplayText))) {
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                }
                case ID_WORK2: {
                    if (ShowDialog2(hwnd, g_DisplayText, sizeof(g_DisplayText))) {
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                }
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            TextOutA(hdc, 50, 50, g_DisplayText, lstrlenA(g_DisplayText));
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXA wc = {0};
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCEA(IDR_MENU1);
    wc.lpszClassName = g_szClassName;

    if(!RegisterClassExA(&wc)) return 0;

    hwnd = CreateWindowExA(0, g_szClassName, "Lab 1 - OOP", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessageA(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return Msg.wParam;
}