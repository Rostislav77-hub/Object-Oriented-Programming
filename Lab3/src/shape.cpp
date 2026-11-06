// src/shape.cpp
#include "shape.h"
#include <algorithm>

void Shape::Set(long x1, long y1, long x2, long y2) {
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}

void PointShape::Show(HDC hdc) {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            SetPixel(hdc, xs1 + dx, ys1 + dy, RGB(0, 0, 0));
        }
    }
}

void LineShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));

    MoveToEx(hdc, xs1, ys1, nullptr);
    LineTo(hdc, xs2, ys2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void RectShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    long left   = std::min(xs1, xs2);
    long right  = std::max(xs1, xs2);
    long top    = std::min(ys1, ys2);
    long bottom = std::max(ys1, ys2);
    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

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