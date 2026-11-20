#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <functional>
#include "editor.h"
#include "shape.h"

constexpr int MY_SHAPE_ARRAY_SIZE = 116;

class MyEditor : public Editor {
public:
    using ChangeCallback = std::function<void()>;

private:
    // Singleton Меєрса (непарний варіант 15): приватні конструктор і деструктор, заборона копіювання
    MyEditor() = default;
    ~MyEditor() override;
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;
    MyEditor(MyEditor&&) = delete;
    MyEditor& operator=(MyEditor&&) = delete;

    static Shape* pcshape[MY_SHAPE_ARRAY_SIZE];
    static int shapeCount;

    std::vector<Shape*> prototypes;   // Реєстр прототипів для розширюваного завантаження файлів
    Shape* currentProto = nullptr;
    int selectedIndex = -1;

    HWND hWndToolbar = nullptr;
    HBITMAP hToolbarBmp = nullptr;
    UINT currentToolId = 0;

    long x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    bool isDragging = false;
    bool hasRubberBand = false;

    ChangeCallback onChanged;

    void UpdateToolbarState();
    void NotifyChanged();
    static void FreeArray();

public:
    // Точка доступу до єдиного екземпляра (Singleton Меєрса)
    static MyEditor& getInstance() {
        static MyEditor instance;
        return instance;
    }

    void RegisterPrototype(Shape* proto);
    void SetOnChanged(ChangeCallback cb) { onChanged = std::move(cb); }

    void Start(Shape* proto, UINT toolId);

    void OnCreate(HWND hWnd, HINSTANCE hInst);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam, bool isTableOpen);

    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
    void OnPaint(HWND hWnd) override;

    // Керування фігурами (таблиця, бонуси 1–3, файли)
    int GetShapeCount() const { return shapeCount; }
    const Shape* GetShape(int index) const;
    int GetSelectedIndex() const { return selectedIndex; }

    void SelectShape(int index, HWND hWnd);
    void DeleteShape(int index, HWND hWnd);
    void ClearAll(HWND hWnd);

    bool SaveToFile(const std::wstring& filePath) const;
    bool LoadFromFile(const std::wstring& filePath, HWND hWnd);
};