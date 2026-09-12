#include "module1.h"
#include "resource.h"

namespace {

const wchar_t* const kGroups[] = {
    L"ІМ-55", L"ІП-51", L"ІА-53", L"ІС-52"
};
const int kGroupsCount = static_cast<int>(sizeof(kGroups) / sizeof(kGroups[0]));

struct DialogParams {
    wchar_t* buffer;
    int bufferSize;
};

INT_PTR CALLBACK Robota1DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        SetWindowLongPtrW(hDlg, DWLP_USER, static_cast<LONG_PTR>(lParam));
        HWND hList = GetDlgItem(hDlg, IDC_LIST_GROUPS);
        for (int i = 0; i < kGroupsCount; ++i)
            SendMessageW(hList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(kGroups[i]));
        SendMessageW(hList, LB_SETCURSEL, 0, 0);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            auto* params = reinterpret_cast<DialogParams*>(GetWindowLongPtrW(hDlg, DWLP_USER));
            HWND hList = GetDlgItem(hDlg, IDC_LIST_GROUPS);
            int sel = static_cast<int>(SendMessageW(hList, LB_GETCURSEL, 0, 0));
            if (sel != LB_ERR && params && params->buffer && params->bufferSize > 0) {
                int len = static_cast<int>(SendMessageW(hList, LB_GETTEXTLEN, sel, 0));
                if (len >= 0 && len < params->bufferSize)
                    SendMessageW(hList, LB_GETTEXT, sel, reinterpret_cast<LPARAM>(params->buffer));
                else
                    params->buffer[0] = L'\0';
            }
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

} 

int ShowRobota1Dialog(HWND hParentWnd, HINSTANCE hInstance,
                       wchar_t* outBuffer, int outBufferSize) {
    DialogParams params{ outBuffer, outBufferSize };
    return static_cast<int>(DialogBoxParamW(
        hInstance, MAKEINTRESOURCEW(IDD_DLG_ROBOTA1), hParentWnd,
        Robota1DialogProc, reinterpret_cast<LPARAM>(&params)));
}