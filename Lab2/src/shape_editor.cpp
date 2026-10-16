#include "shape_editor.h"

Shape** ShapeEditor::pcshape = nullptr;
int ShapeEditor::shapeCount = 0;

void ShapeEditor::InitArray() {
    if (!pcshape) {
        pcshape = new Shape*[MY_SHAPE_ARRAY_SIZE];
        for (int i = 0; i < MY_SHAPE_ARRAY_SIZE; ++i) {
            pcshape[i] = nullptr;
        }
        shapeCount = 0;
    }
}

void ShapeEditor::FreeArray() {
    if (pcshape) {
        for (int i = 0; i < shapeCount; ++i) {
            delete pcshape[i];
        }
        delete[] pcshape;
        pcshape = nullptr;
        shapeCount = 0;
    }
}

void ShapeEditor::DrawAllShapes(HDC hdc) {
    if (!pcshape) return;
    for (int i = 0; i < shapeCount; ++i) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdc);
        }
    }
}

HPEN ShapeEditor::CreateRubberPen() const {
    return CreatePen(PS_DOT, 1, RGB(0, 0, 0));
}

void ShapeEditor::OnLBdown(HWND hWnd) {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x1 = x2 = pt.x;
    y1 = y2 = pt.y;
    isDragging = true;
    SetCapture(hWnd);
}

void ShapeEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    DrawAllShapes(hdc);
    EndPaint(hWnd, &ps);
}

void PointEditor::OnMouseMove(HWND) {
}

void PointEditor::OnLBup(HWND hWnd) {
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = pt.y;

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

    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = pt.y;

    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);

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
    y2 = pt.y;

    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
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

    Rectangle(hdc, 2 * x1 - x2, 2 * y1 - y2, x2, y2);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = pt.y;

    Rectangle(hdc, 2 * x1 - x2, 2 * y1 - y2, x2, y2);

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
    y2 = pt.y;

    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
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

    Ellipse(hdc, x1, y1, x2, y2);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = pt.y;

    Ellipse(hdc, x1, y1, x2, y2);

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
    y2 = pt.y;

    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
        pcshape[shapeCount] = new EllipseShape();
        pcshape[shapeCount]->Set(x1, y1, x2, y2);
        ++shapeCount;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

ShapeObjectsEditor::ShapeObjectsEditor() {
    ShapeEditor::InitArray();
    pse = nullptr;
}

ShapeObjectsEditor::~ShapeObjectsEditor() {
    delete pse;
    ShapeEditor::FreeArray();
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd) {
    delete pse;
    pse = new PointEditor();
    SetWindowTextW(hWnd, L"OOP_lab2 — Крапка");
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd) {
    delete pse;
    pse = new LineEditor();
    SetWindowTextW(hWnd, L"OOP_lab2 — Лінія");
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd) {
    delete pse;
    pse = new RectEditor();
    SetWindowTextW(hWnd, L"OOP_lab2 — Прямокутник");
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd) {
    delete pse;
    pse = new EllipseEditor();
    SetWindowTextW(hWnd, L"OOP_lab2 — Еліпс");
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