#pragma once
#include <windows.h>
#include <string>
#include <functional>

class MyTable {
public:
    using SelectCallback = std::function<void(int)>;
    using DeleteCallback = std::function<void(int)>;
    using CloseCallback  = std::function<void()>;

private:
    HWND hDlg = nullptr;
    HWND hList = nullptr;

    UINT dialogResId = 0;
    int listCtrlId = 0;
    int deleteBtnId = 0;
    int clearSelBtnId = 0;

    bool isUpdating = false;

    SelectCallback onSelect;
    DeleteCallback onDelete;
    CloseCallback  onClose;

    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    INT_PTR HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void InitListColumns();
    void LayoutControls(int width, int height);
    int GetSelectedIndex() const;

public:
    MyTable() = default;
    ~MyTable();

    void ConfigureIds(UINT dlgId, int listId, int delBtnId, int clearBtnId);
    void SetOnSelect(SelectCallback cb) { onSelect = std::move(cb); }
    void SetOnDelete(DeleteCallback cb) { onDelete = std::move(cb); }
    void SetOnClose(CloseCallback cb)   { onClose  = std::move(cb); }

    void Open(HINSTANCE hInst, HWND hParent);
    void Close();
    bool IsOpen() const { return hDlg != nullptr && IsWindow(hDlg); }
    HWND GetHwnd() const { return hDlg; }

    void Clear();
    void AddRow(int index, const std::wstring& name, long x1, long y1, long x2, long y2);
    void SetSelectedRow(int index);
};