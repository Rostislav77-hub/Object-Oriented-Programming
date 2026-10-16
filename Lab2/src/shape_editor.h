#pragma once
#include <windows.h>
#include "editor.h"
#include "shape.h"

constexpr int MY_SHAPE_ARRAY_SIZE = 115;

class ShapeEditor : public Editor {
protected:
    long x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    bool isDragging = false;

    static Shape** pcshape;
    static int shapeCount;

    HPEN CreateRubberPen() const;

public:
    static void InitArray();
    static void FreeArray();
    static void DrawAllShapes(HDC hdc);

    void OnLBdown(HWND hWnd) override;
    void OnPaint(HWND hWnd) override;
};

class PointEditor : public ShapeEditor {
public:
    void OnMouseMove(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
};

class LineEditor : public ShapeEditor {
public:
    void OnMouseMove(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
};

class RectEditor : public ShapeEditor {
public:
    void OnMouseMove(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
};

class EllipseEditor : public ShapeEditor {
public:
    void OnMouseMove(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
};

class ShapeObjectsEditor {
private:
    ShapeEditor* pse = nullptr;
public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
};