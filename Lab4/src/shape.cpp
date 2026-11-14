#include "shape.h"
#include <algorithm>

void Shape::Set(long x1, long y1, long x2, long y2) {
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}

HPEN Shape::CreateCurrentPen() const {
    return CreatePen(isRubber ? PS_DOT : PS_SOLID, 1, RGB(0, 0, 0));
}

void Shape::ShowRubber(HDC hdc) {
    isRubber = true;
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    Show(hdc);
    SetBkMode(hdc, oldBkMode);
    isRubber = false;
}

void PointShape::Show(HDC hdc) {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            SetPixel(hdc, xs1 + dx, ys1 + dy, RGB(0, 0, 0));
        }
    }
}

void LineShape::Show(HDC hdc) {
    HPEN hPen = CreateCurrentPen();
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));

    MoveToEx(hdc, xs1, ys1, nullptr);
    LineTo(hdc, xs2, ys2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void RectShape::Show(HDC hdc) {
    HPEN hPen = CreateCurrentPen();
    HBRUSH hBrush = (!isRubber && useOrangeFill)
        ? CreateSolidBrush(RGB(255, 165, 0))
        : static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    long left   = std::min(xs1, xs2);
    long right  = std::max(xs1, xs2);
    long top    = std::min(ys1, ys2);
    long bottom = std::max(ys1, ys2);
    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    if (!isRubber && useOrangeFill) {
        DeleteObject(hBrush);
    }
    DeleteObject(hPen);
}

void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreateCurrentPen();
    HBRUSH hBrush = isRubber
        ? static_cast<HBRUSH>(GetStockObject(NULL_BRUSH))
        : static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    long xOpposite = 2 * xs1 - xs2;
    long yOpposite = 2 * ys1 - ys2;
    long left   = std::min(xOpposite, xs2);
    long right  = std::max(xOpposite, xs2);
    long top    = std::min(yOpposite, ys2);
    long bottom = std::max(yOpposite, ys2);
    Ellipse(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void LineOOShape::Show(HDC hdc) {
    long x1 = xs1, y1 = ys1, x2 = xs2, y2 = ys2;
    const long r = 6; 

    Set(x1, y1, x2, y2);
    LineShape::Show(hdc);

    Set(x1, y1, x1 + r, y1 + r);
    EllipseShape::Show(hdc);

    Set(x2, y2, x2 + r, y2 + r);
    EllipseShape::Show(hdc);

    Set(x1, y1, x2, y2);
}

CubeShape::CubeShape() {
    useOrangeFill = false; 
}

void CubeShape::Show(HDC hdc) {
    long x1 = xs1, y1 = ys1, x2 = xs2, y2 = ys2;
    long dx = (x2 - x1) / 3;
    long dy = (y2 - y1) / 3;
    if (dx == 0 && x1 != x2) dx = (x2 > x1) ? 10 : -10;
    if (dy == 0 && y1 != y2) dy = (y2 > y1) ? 10 : -10;

    Set(x1 + dx, y1 + dy, x2 + dx, y2 + dy);
    RectShape::Show(hdc);

    Set(x1, y1, x2, y2);
    RectShape::Show(hdc);

    Set(x1, y1, x1 + dx, y1 + dy);
    LineShape::Show(hdc);

    Set(x2, y1, x2 + dx, y1 + dy);
    LineShape::Show(hdc);

    Set(x1, y2, x1 + dx, y2 + dy);
    LineShape::Show(hdc);

    Set(x2, y2, x2 + dx, y2 + dy);
    LineShape::Show(hdc);

    Set(x1, y1, x2, y2);
}