#pragma once
#include <windows.h>
#include <commctrl.h>
#include "editor.h"
#include "shape.h"

constexpr int MY_SHAPE_ARRAY_SIZE = 116;

class ShapeEditor : public Editor {
protected:
    long x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    bool isDragging = false;

    static Shape* pcshape[MY_SHAPE_ARRAY_SIZE];
    static int shapeCount;

    HPEN CreateRubberPen() const;

public:
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
    HWND hWndToolbar = nullptr;
    UINT currentToolId = 0;

    void UpdateToolbarState();

public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    void OnCreate(HWND hWnd, HINSTANCE hInst);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);

    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
};