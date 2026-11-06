// src/shape.cpp
// Реалізація відображення фігур строго за варіантом Лаб3: Ж = 15 + 1 = 16
#include "shape.h"

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
    // Варіант Ж=16:
    // Ввід: по двом протилежним кутам -> (16 mod 2 = 0)
    // Відображення: чорний контур з кольоровим заповненням -> (16 mod 5 = 1)
    // Колір заповнення: помаранчевий -> (16 mod 6 = 4)
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0)); // Помаранчевий

    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

    Rectangle(hdc, xs1, ys1, xs2, ys2);

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
    Ellipse(hdc, xOpposite, yOpposite, xs2, ys2);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}