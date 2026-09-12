#include "module2.h"
#include "resource.h"

static char* g_buffer = nullptr;
static int g_bufferSize = 0;

static INT_PTR CALLBACK Dialog2Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK) {
                if (g_buffer != nullptr) {
                    GetDlgItemText(hwndDlg, IDC_EDITBOX, g_buffer, g_bufferSize);
                }
                EndDialog(hwndDlg, IDOK);
                return (INT_PTR)TRUE;
            }
            else if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwndDlg, IDCANCEL);
                return (INT_PTR)TRUE;
            }
            break;
        }
    }
    return (INT_PTR)FALSE;
}

bool ShowDialog2(HWND hwndParent, char* buffer, int bufferSize) {
    g_buffer = buffer;
    g_bufferSize = bufferSize;
    return (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwndParent, Dialog2Proc) == IDOK);
}