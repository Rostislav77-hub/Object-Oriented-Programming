#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <string>
#include "resource.h"
#include "my_editor.h"
#include "my_table.h"

namespace {

const wchar_t kClassName[]    = L"Lab5MainWindowClass";
const wchar_t kTitle[]        = L"OOP_lab5 (В=15, ІМ-55)";
const wchar_t kAutoSaveFile[] = L"objects.txt";

HINSTANCE g_hInst = nullptr;
HWND g_hMainWnd = nullptr;
MyTable g_table;

void SyncTableView() {
    if (!g_table.IsOpen()) return;
    MyEditor& editor = MyEditor::getInstance();
    g_table.Clear();
    int count = editor.GetShapeCount();
    for (int i = 0; i < count; ++i) {
        const Shape* s = editor.GetShape(i);
        if (s) {
            g_table.AddRow(i, s->GetName(), s->GetX1(), s->GetY1(), s->GetX2(), s->GetY2());
        }
    }
    g_table.SetSelectedRow(editor.GetSelectedIndex());
}

void HandleOpenFile(HWND hWnd) {
    wchar_t filePath[MAX_PATH] = L"";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = L"Текстові файли (*.txt)\0*.txt\0Усі файли (*.*)\0*.*\0";
    ofn.lpstrFile   = filePath;
    ofn.nMaxFile    = MAX_PATH;
    ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn)) {
        if (!MyEditor::getInstance().LoadFromFile(filePath, hWnd)) {
            MessageBoxW(hWnd, L"Не вдалося відкрити файл.", L"Помилка", MB_OK | MB_ICONERROR);
        }
    }
}

void HandleSaveFile(HWND hWnd) {
    wchar_t filePath[MAX_PATH] = L"objects.txt";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = L"Текстові файли (*.txt)\0*.txt\0Усі файли (*.*)\0*.*\0";
    ofn.lpstrFile   = filePath;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags       = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameW(&ofn)) {
        if (!MyEditor::getInstance().SaveToFile(filePath)) {
            MessageBoxW(hWnd, L"Не вдалося зберегти файл.", L"Помилка", MB_OK | MB_ICONERROR);
        }
    }
}

} 

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MyEditor& editor = MyEditor::getInstance();

    switch (msg) {
    case WM_CREATE:
        g_hMainWnd = hWnd;
        editor.OnCreate(hWnd, g_hInst);

        // Реєстрація прототипів для розширюваного завантаження файлів (без залежності у MyEditor)
        editor.RegisterPrototype(new PointShape());
        editor.RegisterPrototype(new LineShape());
        editor.RegisterPrototype(new RectShape());
        editor.RegisterPrototype(new EllipseShape());
        editor.RegisterPrototype(new LineOOShape());
        editor.RegisterPrototype(new CubeShape());

        // Callback при будь-якій зміні масиву об'єктів: оновлюємо таблицю + автозапис objects.txt
        editor.SetOnChanged([]() {
            SyncTableView();
            MyEditor::getInstance().SaveToFile(kAutoSaveFile);
        });

        // Налаштування незалежного модуля MyTable через Callback-функції (Бонуси 1 і 2)
        g_table.ConfigureIds(IDD_TABLE_DIALOG, IDC_TABLE_LIST, IDC_BTN_DELETE, IDC_BTN_CLEAR_SEL);
        g_table.SetOnSelect([](int index) {
            if (g_hMainWnd) {
                MyEditor::getInstance().SelectShape(index, g_hMainWnd);
            }
        });
        g_table.SetOnDelete([](int index) {
            if (g_hMainWnd) {
                MyEditor::getInstance().DeleteShape(index, g_hMainWnd);
            }
        });
        g_table.SetOnClose([]() {
            if (g_hMainWnd) {
                MyEditor::getInstance().SelectShape(-1, g_hMainWnd);
            }
        });
        return 0;

    case WM_SIZE:
        editor.OnSize(hWnd);
        return 0;

    case WM_NOTIFY:
        editor.OnNotify(hWnd, wParam, lParam);
        return 0;

    case WM_INITMENUPOPUP:
        editor.OnInitMenuPopup(hWnd, wParam, g_table.IsOpen());
        return 0;

    case WM_LBUTTONDOWN:
        editor.OnLBdown(hWnd);
        return 0;

    case WM_LBUTTONUP:
        editor.OnLBup(hWnd);
        return 0;

    case WM_MOUSEMOVE:
        editor.OnMouseMove(hWnd);
        return 0;

    case WM_PAINT:
        editor.OnPaint(hWnd);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OBJ_POINT:   editor.Start(new PointShape(),   ID_OBJ_POINT);   return 0;
        case ID_OBJ_LINE:    editor.Start(new LineShape(),    ID_OBJ_LINE);    return 0;
        case ID_OBJ_RECT:    editor.Start(new RectShape(),    ID_OBJ_RECT);    return 0;
        case ID_OBJ_ELLIPSE: editor.Start(new EllipseShape(), ID_OBJ_ELLIPSE); return 0;
        case ID_OBJ_LINEOO:  editor.Start(new LineOOShape(),  ID_OBJ_LINEOO);  return 0;
        case ID_OBJ_CUBE:    editor.Start(new CubeShape(),    ID_OBJ_CUBE);    return 0;

        case ID_VIEW_TABLE:
            if (g_table.IsOpen()) {
                g_table.Close();
            } else {
                g_table.Open(g_hInst, hWnd);
                SyncTableView();
            }
            return 0;

        case ID_FILE_OPEN:
            HandleOpenFile(hWnd);
            return 0;

        case ID_FILE_SAVE:
            HandleSaveFile(hWnd);
            return 0;

        case ID_FILE_CLEAR:
            editor.ClearAll(hWnd);
            return 0;

        case ID_HELP_ABOUT:
            MessageBoxW(hWnd,
                L"Лабораторна робота №5\nБагатовіконний інтерфейс (Singleton Меєрса + Таблиця)\n"
                L"Автор: Матвіїв Р. С., група ІМ-55 (варіант 15)",
                L"Про програму", MB_OK | MB_ICONINFORMATION);
            return 0;

        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        // Автоматичне закриття немодального вікна таблиці при виході з програми
        g_table.Close();
        g_hMainWnd = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow) {
    g_hInst = hInst;

    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(icex);
    icex.dwICC  = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

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
        CW_USEDEFAULT, CW_USEDEFAULT, 820, 540, nullptr, nullptr, hInst, nullptr);
    if (!hWnd) return 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (g_table.GetHwnd() && IsDialogMessageW(g_table.GetHwnd(), &msg)) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}