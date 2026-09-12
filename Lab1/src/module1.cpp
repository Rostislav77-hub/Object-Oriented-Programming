#include "module1.h"
#include "resource.h"

static char* g_buffer = nullptr;
static int g_bufferSize = 0;

static INT_PTR CALLBACK Dialog1Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG: {
            SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)"²Ì-55");
            SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)"²Ï-51");
            SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)"²À-53");
            SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_ADDSTRING, 0, (LPARAM)"²Ñ-52");
            return (INT_PTR)TRUE;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK) {
                LRESULT sel = SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR && g_buffer != nullptr) {
                    SendDlgItemMessage(hwndDlg, IDC_LISTBOX, LB_GETTEXT, sel, (LPARAM)g_buffer);
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

bool ShowDialog1(HWND hwndParent, char* buffer, int bufferSize) {
    g_buffer = buffer;
    g_bufferSize = bufferSize;
    return (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwndParent, Dialog1Proc) == IDOK);
}