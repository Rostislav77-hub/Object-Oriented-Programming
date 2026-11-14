#pragma once
#include <windows.h>
#include <commctrl.h>
#include "editor.h"
#include "shape.h"

constexpr int MY_SHAPE_ARRAY_SIZE = 116;

class MyEditor : public Editor {
private:
    static Shape* pcshape[MY_SHAPE_ARRAY_SIZE];
    static int shapeCount;

    Shape* currentProto = nullptr;
    HWND hWndToolbar = nullptr;
    HBITMAP hToolbarBmp = nullptr;
    UINT currentToolId = 0;

    long x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    bool isDragging = false;
    bool hasRubberBand = false;

    void UpdateToolbarState();
    static void FreeArray();

public:
    MyEditor();
    ~MyEditor() override;

    void Start(Shape* proto, UINT toolId);

    void OnCreate(HWND hWnd, HINSTANCE hInst);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);

    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
    void OnPaint(HWND hWnd) override;
};