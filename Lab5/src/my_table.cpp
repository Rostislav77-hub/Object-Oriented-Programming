#include "my_table.h"
#include <commctrl.h>

MyTable::~MyTable() {
    Close();
}

void MyTable::ConfigureIds(UINT dlgId, int listId, int delBtnId, int clearBtnId) {
    dialogResId = dlgId;
    listCtrlId = listId;
    deleteBtnId = delBtnId;
    clearSelBtnId = clearBtnId;
}

void MyTable::Open(HINSTANCE hInst, HWND hParent) {
    if (IsOpen()) {
        ShowWindow(hDlg, SW_SHOW);
        SetForegroundWindow(hDlg);
        return;
    }

    hDlg = CreateDialogParamW(
        hInst,
        MAKEINTRESOURCEW(dialogResId),
        hParent,
        DlgProc,
        reinterpret_cast<LPARAM>(this)
    );

    if (hDlg) {
        ShowWindow(hDlg, SW_SHOW);
        UpdateWindow(hDlg);
    }
}

void MyTable::Close() {
    if (hDlg && IsWindow(hDlg)) {
        DestroyWindow(hDlg);
    }
    hDlg = nullptr;
    hList = nullptr;
}

void MyTable::Clear() {
    if (!hList || !IsWindow(hList)) return;
    isUpdating = true;
    ListView_DeleteAllItems(hList);
    isUpdating = false;
}

void MyTable::AddRow(int index, const std::wstring& name, long x1, long y1, long x2, long y2) {
    if (!hList || !IsWindow(hList)) return;

    isUpdating = true;

    std::wstring numStr = std::to_wstring(index + 1);
    LVITEMW item = {};
    item.mask = LVIF_TEXT;
    item.iItem = index;
    item.iSubItem = 0;
    item.pszText = const_cast<LPWSTR>(numStr.c_str());

    int row = ListView_InsertItem(hList, &item);
    if (row >= 0) {
        std::wstring sx1 = std::to_wstring(x1);
        std::wstring sy1 = std::to_wstring(y1);
        std::wstring sx2 = std::to_wstring(x2);
        std::wstring sy2 = std::to_wstring(y2);

        ListView_SetItemText(hList, row, 1, const_cast<LPWSTR>(name.c_str()));
        ListView_SetItemText(hList, row, 2, const_cast<LPWSTR>(sx1.c_str()));
        ListView_SetItemText(hList, row, 3, const_cast<LPWSTR>(sy1.c_str()));
        ListView_SetItemText(hList, row, 4, const_cast<LPWSTR>(sx2.c_str()));
        ListView_SetItemText(hList, row, 5, const_cast<LPWSTR>(sy2.c_str()));
    }

    isUpdating = false;
}

void MyTable::SetSelectedRow(int index) {
    if (!hList || !IsWindow(hList)) return;
    isUpdating = true;
    int count = ListView_GetItemCount(hList);
    for (int i = 0; i < count; ++i) {
        UINT state = (i == index) ? (LVIS_SELECTED | LVIS_FOCUSED) : 0;
        ListView_SetItemState(hList, i, state, LVIS_SELECTED | LVIS_FOCUSED);
    }
    isUpdating = false;
}

int MyTable::GetSelectedIndex() const {
    if (!hList || !IsWindow(hList)) return -1;
    return ListView_GetNextItem(hList, -1, LVNI_SELECTED);
}

void MyTable::InitListColumns() {
    if (!hList) return;
    ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    struct ColDef { const wchar_t* title; int width; };
    const ColDef cols[6] = {
        { L"№",     36 },
        { L"Назва", 155 },
        { L"x1",    62 },
        { L"y1",    62 },
        { L"x2",    62 },
        { L"y2",    62 }
    };

    for (int i = 0; i < 6; ++i) {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        col.pszText = const_cast<LPWSTR>(cols[i].title);
        col.cx = cols[i].width;
        col.iSubItem = i;
        ListView_InsertColumn(hList, i, &col);
    }
}

void MyTable::LayoutControls(int width, int height) {
    if (!hList || width < 100 || height < 80) return;
    const int margin = 8;
    const int btnHeight = 28;
    const int listHeight = height - btnHeight - margin * 3;

    MoveWindow(hList, margin, margin, width - margin * 2, listHeight, TRUE);

    HWND hDel   = GetDlgItem(hDlg, deleteBtnId);
    HWND hClear = GetDlgItem(hDlg, clearSelBtnId);
    HWND hClose = GetDlgItem(hDlg, IDCANCEL);

    int btnY = margin * 2 + listHeight;
    if (hDel)   MoveWindow(hDel,   margin,       btnY, 145, btnHeight, TRUE);
    if (hClear) MoveWindow(hClear, margin + 153, btnY, 130, btnHeight, TRUE);
    if (hClose) MoveWindow(hClose, width - margin - 80, btnY, 80, btnHeight, TRUE);
}

INT_PTR CALLBACK MyTable::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MyTable* self = nullptr;
    if (msg == WM_INITDIALOG) {
        self = reinterpret_cast<MyTable*>(lParam);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        if (self) self->hDlg = hWnd;
    } else {
        self = reinterpret_cast<MyTable*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    if (self) {
        return self->HandleMessage(hWnd, msg, wParam, lParam);
    }
    return FALSE;
}

INT_PTR MyTable::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG: {
        hList = GetDlgItem(hWnd, listCtrlId);
        InitListColumns();
        RECT rc = {};
        GetClientRect(hWnd, &rc);
        LayoutControls(rc.right - rc.left, rc.bottom - rc.top);
        return TRUE;
    }

    case WM_SIZE:
        LayoutControls(LOWORD(lParam), HIWORD(lParam));
        return TRUE;

    case WM_NOTIFY: {
        LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
        if (pnmh && static_cast<int>(pnmh->idFrom) == listCtrlId) {
            if (pnmh->code == LVN_ITEMCHANGED && !isUpdating) {
                LPNMLISTVIEW pnmv = reinterpret_cast<LPNMLISTVIEW>(lParam);
                if (pnmv->uChanged & LVIF_STATE) {
                    bool nowSel = (pnmv->uNewState & LVIS_SELECTED) != 0;
                    bool wasSel = (pnmv->uOldState & LVIS_SELECTED) != 0;
                    if (nowSel && !wasSel) {
                        if (onSelect) onSelect(pnmv->iItem);
                    } else if (wasSel && !nowSel) {
                        if (GetSelectedIndex() == -1 && onSelect) {
                            onSelect(-1);
                        }
                    }
                }
            } else if (pnmh->code == LVN_KEYDOWN) {
                LPNMLVKEYDOWN pkd = reinterpret_cast<LPNMLVKEYDOWN>(lParam);
                if (pkd->wVKey == VK_DELETE) {
                    int idx = GetSelectedIndex();
                    if (idx >= 0 && onDelete) onDelete(idx);
                }
            }
        }
        return TRUE;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == deleteBtnId) {
            int idx = GetSelectedIndex();
            if (idx >= 0 && onDelete) onDelete(idx);
            return TRUE;
        }
        if (id == clearSelBtnId) {
            SetSelectedRow(-1);
            if (onSelect) onSelect(-1);
            return TRUE;
        }
        if (id == IDCANCEL) {
            DestroyWindow(hWnd);
            return TRUE;
        }
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;

    case WM_DESTROY:
        hDlg = nullptr;
        hList = nullptr;
        if (onClose) onClose();
        return TRUE;
    }
    return FALSE;
}