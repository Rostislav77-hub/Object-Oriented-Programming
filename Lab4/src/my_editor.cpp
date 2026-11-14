#include "my_editor.h"
#include "resource.h"
#include <algorithm>

namespace {

HBITMAP CreateToolbarIcons(HWND hWnd) {
    HDC hdcScreen = GetDC(hWnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 96, 16);
    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hdcMem, hBmp));

    RECT rcFull = { 0, 0, 96, 16 };
    FillRect(hdcMem, &rcFull, GetSysColorBrush(COLOR_BTNFACE));

    HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdcMem, hBlackPen));

    RECT rcDot = { 6, 6, 10, 10 };
    FillRect(hdcMem, &rcDot, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

    MoveToEx(hdcMem, 19, 13, nullptr);
    LineTo(hdcMem, 29, 3);

    HBRUSH hOrangeBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdcMem, hOrangeBrush));
    Rectangle(hdcMem, 34, 3, 46, 13);

    SelectObject(hdcMem, GetStockObject(WHITE_BRUSH));
    Ellipse(hdcMem, 50, 3, 62, 13);

    MoveToEx(hdcMem, 67, 12, nullptr);
    LineTo(hdcMem, 77, 4);
    Ellipse(hdcMem, 65, 10, 70, 15);
    Ellipse(hdcMem, 74, 2, 79, 7);

    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 82, 6, 90, 14);
    Rectangle(hdcMem, 85, 2, 93, 10);
    MoveToEx(hdcMem, 82, 6, nullptr);  LineTo(hdcMem, 85, 2);
    MoveToEx(hdcMem, 90, 6, nullptr);  LineTo(hdcMem, 93, 2);
    MoveToEx(hdcMem, 82, 14, nullptr); LineTo(hdcMem, 85, 10);
    MoveToEx(hdcMem, 90, 14, nullptr); LineTo(hdcMem, 93, 10);

    SelectObject(hdcMem, hOldBrush);
    DeleteObject(hOrangeBrush);
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBlackPen);

    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hWnd, hdcScreen);
    return hBmp;
}

} 

Shape* MyEditor::pcshape[MY_SHAPE_ARRAY_SIZE] = { nullptr };
int MyEditor::shapeCount = 0;

MyEditor::MyEditor() = default;

MyEditor::~MyEditor() {
    delete currentProto;
    currentProto = nullptr;
    if (hToolbarBmp) {
        DeleteObject(hToolbarBmp);
        hToolbarBmp = nullptr;
    }
    FreeArray();
}

void MyEditor::FreeArray() {
    for (int i = 0; i < shapeCount; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

void MyEditor::Start(Shape* proto, UINT toolId) {
    delete currentProto;
    currentProto = proto;
    currentToolId = toolId;
    UpdateToolbarState();
}

void MyEditor::OnCreate(HWND hWnd, HINSTANCE hInst) {
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    if (hComCtl) {
        using InitFn = void (WINAPI*)();
        auto pInit = reinterpret_cast<InitFn>(GetProcAddress(hComCtl, "InitCommonControls"));
        if (pInit) pInit();
    }

    hWndToolbar = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_MY_TOOLBAR)),
        hInst,
        nullptr
    );

    if (!hWndToolbar) return;

    SendMessageW(hWndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessageW(hWndToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(16, 16));
    SendMessageW(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(24, 24));

    hToolbarBmp = CreateToolbarIcons(hWnd);

    TBADDBITMAP tbab = {};
    tbab.hInst = nullptr;
    tbab.nID   = reinterpret_cast<UINT_PTR>(hToolbarBmp);
    SendMessageW(hWndToolbar, TB_ADDBITMAP, 6, reinterpret_cast<LPARAM>(&tbab));

    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };

    TBBUTTON tbb[6] = {};
    for (int i = 0; i < 6; ++i) {
        tbb[i].iBitmap   = i;
        tbb[i].idCommand = kCommands[i];
        tbb[i].fsState   = TBSTATE_ENABLED;
        tbb[i].fsStyle   = TBSTYLE_BUTTON;
    }

    SendMessageW(hWndToolbar, TB_ADDBUTTONSW, 6, reinterpret_cast<LPARAM>(tbb));
    SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
}

void MyEditor::OnSize(HWND) {
    if (hWndToolbar) {
        SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
    }
}

void MyEditor::OnNotify(HWND, WPARAM, LPARAM lParam) {
    LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
    if (pnmh->code == TTN_NEEDTEXTW) {
        LPNMTTDISPINFOW lpttt = reinterpret_cast<LPNMTTDISPINFOW>(lParam);
        switch (lpttt->hdr.idFrom) {
        case ID_OBJ_POINT:   lstrcpyW(lpttt->szText, L"Крапка");             break;
        case ID_OBJ_LINE:    lstrcpyW(lpttt->szText, L"Лінія");              break;
        case ID_OBJ_RECT:    lstrcpyW(lpttt->szText, L"Прямокутник");        break;
        case ID_OBJ_ELLIPSE: lstrcpyW(lpttt->szText, L"Еліпс");              break;
        case ID_OBJ_LINEOO:  lstrcpyW(lpttt->szText, L"Лінія з кружечками"); break;
        case ID_OBJ_CUBE:    lstrcpyW(lpttt->szText, L"Каркас куба");        break;
        }
    }
}

void MyEditor::OnInitMenuPopup(HWND hWnd, WPARAM) {
    HMENU hMenu = GetMenu(hWnd);
    if (!hMenu) return;

    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };
    for (UINT cmd : kCommands) {
        CheckMenuItem(hMenu, cmd, (currentToolId == cmd) ? MF_CHECKED : MF_UNCHECKED);
    }
}

void MyEditor::UpdateToolbarState() {
    if (!hWndToolbar) return;
    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };
    for (UINT cmd : kCommands) {
        SendMessageW(hWndToolbar, TB_PRESSBUTTON, cmd, MAKELONG(currentToolId == cmd, 0));
    }
}

void MyEditor::OnLBdown(HWND hWnd) {
    if (!currentProto) return;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    if (pt.y < 32) return;

    x1 = x2 = pt.x;
    y1 = y2 = pt.y;
    isDragging = true;
    hasRubberBand = false;
    SetCapture(hWnd);
}

void MyEditor::OnMouseMove(HWND hWnd) {
    if (!isDragging || !currentProto || currentToolId == ID_OBJ_POINT) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);

    if (hasRubberBand) {
        currentProto->Set(x1, y1, x2, y2);
        currentProto->ShowRubber(hdc);
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    currentProto->Set(x1, y1, x2, y2);
    currentProto->ShowRubber(hdc);
    hasRubberBand = true;

    ReleaseDC(hWnd, hdc);
}

void MyEditor::OnLBup(HWND hWnd) {
    if (!isDragging || !currentProto) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
        if (currentToolId == ID_OBJ_POINT) {
            Shape* s = currentProto->Clone();
            s->Set(x2, y2, x2, y2);
            pcshape[shapeCount++] = s;
        } else if (x1 != x2 || y1 != y2) {
            Shape* s = currentProto->Clone();
            s->Set(x1, y1, x2, y2);
            pcshape[shapeCount++] = s;
        }
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    for (int i = 0; i < shapeCount; ++i) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdc);
        }
    }
    EndPaint(hWnd, &ps);
}