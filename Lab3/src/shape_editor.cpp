#include "shape_editor.h"
#include "resource.h"
#include <algorithm>

namespace {

HBITMAP CreateToolbarIcons(HWND hWnd) {
    HDC hdcScreen = GetDC(hWnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 64, 16);
    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hdcMem, hBmp));

    RECT rcFull = { 0, 0, 64, 16 };
    FillRect(hdcMem, &rcFull, GetSysColorBrush(COLOR_BTNFACE));

    HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdcMem, hBlackPen));

    RECT rcDot = { 6, 6, 10, 10 };
    FillRect(hdcMem, &rcDot, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

    MoveToEx(hdcMem, 19, 13, nullptr);
    LineTo(hdcMem, 29, 3);
    MoveToEx(hdcMem, 19, 12, nullptr);
    LineTo(hdcMem, 28, 3);

    HBRUSH hOrangeBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdcMem, hOrangeBrush));
    Rectangle(hdcMem, 34, 3, 46, 13);

    SelectObject(hdcMem, GetStockObject(WHITE_BRUSH));
    Ellipse(hdcMem, 50, 3, 62, 13);

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

Shape* ShapeEditor::pcshape[MY_SHAPE_ARRAY_SIZE] = { nullptr };
int ShapeEditor::shapeCount = 0;

void ShapeEditor::FreeArray() {
    for (int i = 0; i < shapeCount; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

void ShapeEditor::DrawAllShapes(HDC hdc) {
    for (int i = 0; i < shapeCount; ++i) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdc);
        }
    }
}

HPEN ShapeEditor::CreateRubberPen() const {
    return CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
}

void ShapeEditor::OnLBdown(HWND hWnd) {
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

void ShapeEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    DrawAllShapes(hdc);
    EndPaint(hWnd, &ps);
}

void PointEditor::OnMouseMove(HWND) {}

void PointEditor::OnLBup(HWND hWnd) {
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
        pcshape[shapeCount] = new PointShape();
        pcshape[shapeCount]->Set(x2, y2, x2, y2);
        ++shapeCount;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void LineEditor::OnMouseMove(HWND hWnd) {
    if (!isDragging) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreateRubberPen();
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));

    if (hasRubberBand) {
        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
    hasRubberBand = true;

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void LineEditor::OnLBup(HWND hWnd) {
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    if ((x1 != x2 || y1 != y2) && shapeCount < MY_SHAPE_ARRAY_SIZE) {
        pcshape[shapeCount] = new LineShape();
        pcshape[shapeCount]->Set(x1, y1, x2, y2);
        ++shapeCount;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void RectEditor::OnMouseMove(HWND hWnd) {
    if (!isDragging) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreateRubberPen();
    HBRUSH hBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    if (hasRubberBand) {
        Rectangle(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    Rectangle(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));
    hasRubberBand = true;

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void RectEditor::OnLBup(HWND hWnd) {
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    if ((x1 != x2 && y1 != y2) && shapeCount < MY_SHAPE_ARRAY_SIZE) {
        pcshape[shapeCount] = new RectShape();
        pcshape[shapeCount]->Set(x1, y1, x2, y2);
        ++shapeCount;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void EllipseEditor::OnMouseMove(HWND hWnd) {
    if (!isDragging) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreateRubberPen();
    HBRUSH hBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    if (hasRubberBand) {
        long xOpp = 2 * x1 - x2;
        long yOpp = 2 * y1 - y2;
        Ellipse(hdc, std::min(xOpp, x2), std::min(yOpp, y2), std::max(xOpp, x2), std::max(yOpp, y2));
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    long xOpp = 2 * x1 - x2;
    long yOpp = 2 * y1 - y2;
    Ellipse(hdc, std::min(xOpp, x2), std::min(yOpp, y2), std::max(xOpp, x2), std::max(yOpp, y2));
    hasRubberBand = true;

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void EllipseEditor::OnLBup(HWND hWnd) {
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    if ((x1 != x2 && y1 != y2) && shapeCount < MY_SHAPE_ARRAY_SIZE) {
        pcshape[shapeCount] = new EllipseShape();
        pcshape[shapeCount]->Set(x1, y1, x2, y2);
        ++shapeCount;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

ShapeObjectsEditor::ShapeObjectsEditor() = default;

ShapeObjectsEditor::~ShapeObjectsEditor() {
    delete pse;
    if (hToolbarBmp) {
        DeleteObject(hToolbarBmp);
        hToolbarBmp = nullptr;
    }
    ShapeEditor::FreeArray();
}

void ShapeObjectsEditor::OnCreate(HWND hWnd, HINSTANCE hInst) {
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
    SendMessageW(hWndToolbar, TB_ADDBITMAP, 4, reinterpret_cast<LPARAM>(&tbab));

    TBBUTTON tbb[4] = {};
    tbb[0].iBitmap   = 0;
    tbb[0].idCommand = ID_OBJ_POINT;
    tbb[0].fsState   = TBSTATE_ENABLED;
    tbb[0].fsStyle   = TBSTYLE_BUTTON;

    tbb[1].iBitmap   = 1;
    tbb[1].idCommand = ID_OBJ_LINE;
    tbb[1].fsState   = TBSTATE_ENABLED;
    tbb[1].fsStyle   = TBSTYLE_BUTTON;

    tbb[2].iBitmap   = 2;
    tbb[2].idCommand = ID_OBJ_RECT;
    tbb[2].fsState   = TBSTATE_ENABLED;
    tbb[2].fsStyle   = TBSTYLE_BUTTON;

    tbb[3].iBitmap   = 3;
    tbb[3].idCommand = ID_OBJ_ELLIPSE;
    tbb[3].fsState   = TBSTATE_ENABLED;
    tbb[3].fsStyle   = TBSTYLE_BUTTON;

    SendMessageW(hWndToolbar, TB_ADDBUTTONSW, 4, reinterpret_cast<LPARAM>(tbb));
    SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
}

void ShapeObjectsEditor::OnSize(HWND) {
    if (hWndToolbar) {
        SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
    }
}

void ShapeObjectsEditor::OnNotify(HWND, WPARAM, LPARAM lParam) {
    LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
    if (pnmh->code == TTN_NEEDTEXTW) {
        LPNMTTDISPINFOW lpttt = reinterpret_cast<LPNMTTDISPINFOW>(lParam);
        switch (lpttt->hdr.idFrom) {
        case ID_OBJ_POINT:
            lstrcpyW(lpttt->szText, L"Крапка");
            break;
        case ID_OBJ_LINE:
            lstrcpyW(lpttt->szText, L"Лінія");
            break;
        case ID_OBJ_RECT:
            lstrcpyW(lpttt->szText, L"Прямокутник");
            break;
        case ID_OBJ_ELLIPSE:
            lstrcpyW(lpttt->szText, L"Еліпс");
            break;
        }
    }
}

void ShapeObjectsEditor::OnInitMenuPopup(HWND hWnd, WPARAM) {
    HMENU hMenu = GetMenu(hWnd);
    if (!hMenu) return;

    CheckMenuItem(hMenu, ID_OBJ_POINT,   (currentToolId == ID_OBJ_POINT)   ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJ_LINE,    (currentToolId == ID_OBJ_LINE)    ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJ_RECT,    (currentToolId == ID_OBJ_RECT)    ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJ_ELLIPSE, (currentToolId == ID_OBJ_ELLIPSE) ? MF_CHECKED : MF_UNCHECKED);
}

void ShapeObjectsEditor::UpdateToolbarState() {
    if (!hWndToolbar) return;
    SendMessageW(hWndToolbar, TB_PRESSBUTTON, ID_OBJ_POINT,   MAKELONG(currentToolId == ID_OBJ_POINT, 0));
    SendMessageW(hWndToolbar, TB_PRESSBUTTON, ID_OBJ_LINE,    MAKELONG(currentToolId == ID_OBJ_LINE, 0));
    SendMessageW(hWndToolbar, TB_PRESSBUTTON, ID_OBJ_RECT,    MAKELONG(currentToolId == ID_OBJ_RECT, 0));
    SendMessageW(hWndToolbar, TB_PRESSBUTTON, ID_OBJ_ELLIPSE, MAKELONG(currentToolId == ID_OBJ_ELLIPSE, 0));
}

void ShapeObjectsEditor::StartPointEditor(HWND) {
    delete pse;
    pse = new PointEditor();
    currentToolId = ID_OBJ_POINT;
    UpdateToolbarState();
}

void ShapeObjectsEditor::StartLineEditor(HWND) {
    delete pse;
    pse = new LineEditor();
    currentToolId = ID_OBJ_LINE;
    UpdateToolbarState();
}

void ShapeObjectsEditor::StartRectEditor(HWND) {
    delete pse;
    pse = new RectEditor();
    currentToolId = ID_OBJ_RECT;
    UpdateToolbarState();
}

void ShapeObjectsEditor::StartEllipseEditor(HWND) {
    delete pse;
    pse = new EllipseEditor();
    currentToolId = ID_OBJ_ELLIPSE;
    UpdateToolbarState();
}

void ShapeObjectsEditor::OnLBdown(HWND hWnd) {
    if (pse) pse->OnLBdown(hWnd);
}

void ShapeObjectsEditor::OnLBup(HWND hWnd) {
    if (pse) pse->OnLBup(hWnd);
}

void ShapeObjectsEditor::OnMouseMove(HWND hWnd) {
    if (pse) pse->OnMouseMove(hWnd);
}

void ShapeObjectsEditor::OnPaint(HWND hWnd) {
    if (pse) {
        pse->OnPaint(hWnd);
    } else {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        ShapeEditor::DrawAllShapes(hdc);
        EndPaint(hWnd, &ps);
    }
}