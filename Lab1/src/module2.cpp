#include "module2.h"
#include "resource.h"

namespace {

struct DialogParams {
    wchar_t* buffer;
    int bufferSize;
};

INT_PTR CALLBACK Robota2DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        SetWindowLongPtrW(hDlg, DWLP_USER, static_cast<LONG_PTR>(lParam));
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            auto* params = reinterpret_cast<DialogParams*>(GetWindowLongPtrW(hDlg, DWLP_USER));
            if (params && params->buffer && params->bufferSize > 0)
                GetDlgItemTextW(hDlg, IDC_EDIT_TEXT, params->buffer, params->bufferSize);
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

int ShowRobota2Dialog(HWND hParentWnd, HINSTANCE hInstance,
                       wchar_t* outBuffer, int outBufferSize) {
    DialogParams params{ outBuffer, outBufferSize };
    return static_cast<int>(DialogBoxParamW(
        hInstance, MAKEINTRESOURCEW(IDD_DLG_ROBOTA2), hParentWnd,
        Robota2DialogProc, reinterpret_cast<LPARAM>(&params)));
}