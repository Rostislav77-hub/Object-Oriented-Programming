#include "shape.h"
#include <algorithm>

void Shape::Set(long x1, long y1, long x2, long y2) {
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}

HPEN Shape::CreateCurrentPen() const {
    if (isRubber) {
        return CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    }
    if (isSelected) {
        return CreatePen(PS_SOLID, 3, RGB(220, 20, 60));
    }
    return CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
}

void Shape::ShowRubber(HDC hdc) {
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    isRubber = true;
    Show(hdc);
    isRubber = false;
    SetBkMode(hdc, oldBkMode);
}

void PointShape::Show(HDC hdc) {
    int radius = isSelected ? 3 : 1;
    COLORREF color = isSelected ? RGB(220, 20, 60) : RGB(0, 0, 0);
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            SetPixel(hdc, xs1 + dx, ys1 + dy, color);
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

    Rectangle(hdc, std::min(xs1, xs2), std::min(ys1, ys2),
                   std::max(xs1, xs2), std::max(ys1, ys2));

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    if (!isRubber && useOrangeFill) DeleteObject(hBrush);
    DeleteObject(hPen);
}

void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreateCurrentPen();
    HBRUSH hBrush = static_cast<HBRUSH>(GetStockObject(isRubber ? NULL_BRUSH : WHITE_BRUSH));

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    long xOpp = 2 * xs1 - xs2;
    long yOpp = 2 * ys1 - ys2;
    Ellipse(hdc, std::min(xOpp, xs2), std::min(yOpp, ys2),
                 std::max(xOpp, xs2), std::max(yOpp, ys2));

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

void CubeShape::Show(HDC hdc) {
    long x1 = std::min(xs1, xs2), y1 = std::min(ys1, ys2);
    long x2 = std::max(xs1, xs2), y2 = std::max(ys1, ys2);
    long origX1 = xs1, origY1 = ys1, origX2 = xs2, origY2 = ys2;

    long dx = std::max(12L, (x2 - x1) / 4);
    long dy = std::max(12L, (y2 - y1) / 4);

    Set(x1 - dx, y1 - dy, x2 - dx, y2 - dy); RectShape::Show(hdc);
    Set(x1,      y1,      x2,      y2);      RectShape::Show(hdc);

    Set(x1 - dx, y1 - dy, x1, y1); LineShape::Show(hdc);
    Set(x2 - dx, y1 - dy, x2, y1); LineShape::Show(hdc);
    Set(x1 - dx, y2 - dy, x1, y2); LineShape::Show(hdc);
    Set(x2 - dx, y2 - dy, x2, y2); LineShape::Show(hdc);

    Set(origX1, origY1, origX2, origY2);
}