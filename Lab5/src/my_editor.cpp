#include "my_editor.h"
#include "resource.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

HBITMAP CreateToolbarIcons(HWND hWnd) {
    HDC hdcScreen = GetDC(hWnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 96, 16);
    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hdcMem, hBmp));

    RECT rcFull = { 0, 0, 96, 16 };
    FillRect(hdcMem, &rcFull, GetSysColorBrush(COLOR_BTNFACE));

    HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdcMem, hBlackPen));

    // 1. Крапка
    RECT rcDot = { 6, 6, 10, 10 };
    FillRect(hdcMem, &rcDot, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

    // 2. Лінія
    MoveToEx(hdcMem, 19, 13, nullptr);
    LineTo(hdcMem, 29, 3);

    // 3. Помаранчевий прямокутник
    HBRUSH hOrangeBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdcMem, hOrangeBrush));
    Rectangle(hdcMem, 34, 3, 46, 13);

    // 4. Білий еліпс
    SelectObject(hdcMem, GetStockObject(WHITE_BRUSH));
    Ellipse(hdcMem, 50, 3, 62, 13);

    // 5. Лінія з кружечками
    MoveToEx(hdcMem, 67, 12, nullptr);
    LineTo(hdcMem, 77, 4);
    Ellipse(hdcMem, 65, 10, 70, 15);
    Ellipse(hdcMem, 74, 2, 79, 7);

    // 6. Каркас куба
    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 82, 6, 90, 14);
    Rectangle(hdcMem, 85, 2, 93, 10);
    MoveToEx(hdcMem, 82, 6, nullptr);  LineTo(hdcMem, 85, 2);
    MoveToEx(hdcMem, 90, 6, nullptr);  LineTo(hdcMem, 93, 2);
    MoveToEx(hdcMem, 82, 14, nullptr); LineTo(hdcMem, 85, 10);
    MoveToEx(hdcMem, 90, 14, nullptr); LineTo(hdcMem, 93, 10);

    SelectObject(hdcMem, hOldBrush);
    DeleteObject(hOrangeBrush);
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBlackPen);

    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hWnd, hdcScreen);
    return hBmp;
}

std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size <= 1) return {};
    std::string out(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, out.data(), size, nullptr, nullptr);
    return out;
}

std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (size <= 1) return {};
    std::wstring out(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, out.data(), size);
    return out;
}

} 

Shape* MyEditor::pcshape[MY_SHAPE_ARRAY_SIZE] = { nullptr };
int MyEditor::shapeCount = 0;

MyEditor::~MyEditor() {
    delete currentProto;
    currentProto = nullptr;

    for (Shape* p : prototypes) {
        delete p;
    }
    prototypes.clear();

    if (hToolbarBmp) {
        DeleteObject(hToolbarBmp);
        hToolbarBmp = nullptr;
    }
    FreeArray();
}

void MyEditor::FreeArray() {
    for (int i = 0; i < shapeCount; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

void MyEditor::RegisterPrototype(Shape* proto) {
    if (proto) prototypes.push_back(proto);
}

void MyEditor::NotifyChanged() {
    if (onChanged) onChanged();
}

void MyEditor::Start(Shape* proto, UINT toolId) {
    delete currentProto;
    currentProto = proto;
    currentToolId = toolId;
    UpdateToolbarState();
}

void MyEditor::OnCreate(HWND hWnd, HINSTANCE hInst) {
    hWndToolbar = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_MY_TOOLBAR)),
        hInst,
        nullptr
    );

    if (!hWndToolbar) return;

    SendMessageW(hWndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessageW(hWndToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(16, 16));
    SendMessageW(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(24, 24));

    hToolbarBmp = CreateToolbarIcons(hWnd);

    TBADDBITMAP tbab = {};
    tbab.hInst = nullptr;
    tbab.nID   = reinterpret_cast<UINT_PTR>(hToolbarBmp);
    SendMessageW(hWndToolbar, TB_ADDBITMAP, 6, reinterpret_cast<LPARAM>(&tbab));

    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };

    TBBUTTON tbb[6] = {};
    for (int i = 0; i < 6; ++i) {
        tbb[i].iBitmap   = i;
        tbb[i].idCommand = kCommands[i];
        tbb[i].fsState   = TBSTATE_ENABLED;
        tbb[i].fsStyle   = TBSTYLE_BUTTON;
    }

    SendMessageW(hWndToolbar, TB_ADDBUTTONSW, 6, reinterpret_cast<LPARAM>(tbb));
    SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
}

void MyEditor::OnSize(HWND) {
    if (hWndToolbar) {
        SendMessageW(hWndToolbar, TB_AUTOSIZE, 0, 0);
    }
}

void MyEditor::OnNotify(HWND, WPARAM, LPARAM lParam) {
    LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
    if (pnmh && pnmh->code == TTN_NEEDTEXTW) {
        LPNMTTDISPINFOW lpttt = reinterpret_cast<LPNMTTDISPINFOW>(lParam);
        switch (lpttt->hdr.idFrom) {
        case ID_OBJ_POINT:   lstrcpyW(lpttt->szText, L"Крапка");             break;
        case ID_OBJ_LINE:    lstrcpyW(lpttt->szText, L"Лінія");              break;
        case ID_OBJ_RECT:    lstrcpyW(lpttt->szText, L"Прямокутник");        break;
        case ID_OBJ_ELLIPSE: lstrcpyW(lpttt->szText, L"Еліпс");              break;
        case ID_OBJ_LINEOO:  lstrcpyW(lpttt->szText, L"Лінія з кружечками"); break;
        case ID_OBJ_CUBE:    lstrcpyW(lpttt->szText, L"Каркас куба");        break;
        }
    }
}

void MyEditor::OnInitMenuPopup(HWND hWnd, WPARAM, bool isTableOpen) {
    HMENU hMenu = GetMenu(hWnd);
    if (!hMenu) return;

    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };
    for (UINT cmd : kCommands) {
        CheckMenuItem(hMenu, cmd, (currentToolId == cmd) ? MF_CHECKED : MF_UNCHECKED);
    }
    CheckMenuItem(hMenu, ID_VIEW_TABLE, isTableOpen ? MF_CHECKED : MF_UNCHECKED);
}

void MyEditor::UpdateToolbarState() {
    if (!hWndToolbar) return;
    const UINT kCommands[6] = {
        ID_OBJ_POINT, ID_OBJ_LINE, ID_OBJ_RECT,
        ID_OBJ_ELLIPSE, ID_OBJ_LINEOO, ID_OBJ_CUBE
    };
    for (UINT cmd : kCommands) {
        SendMessageW(hWndToolbar, TB_PRESSBUTTON, cmd, MAKELONG(currentToolId == cmd, 0));
    }
}

void MyEditor::OnLBdown(HWND hWnd) {
    if (!currentProto) return;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    if (pt.y < 32) return;

    x1 = x2 = pt.x;
    y1 = y2 = pt.y;
    isDragging = true;
    hasRubberBand = false;
    SetCapture(hWnd);
}

void MyEditor::OnMouseMove(HWND hWnd) {
    if (!isDragging || !currentProto || currentToolId == ID_OBJ_POINT) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);

    if (hasRubberBand) {
        currentProto->Set(x1, y1, x2, y2);
        currentProto->ShowRubber(hdc);
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    currentProto->Set(x1, y1, x2, y2);
    currentProto->ShowRubber(hdc);
    hasRubberBand = true;

    ReleaseDC(hWnd, hdc);
}

void MyEditor::OnLBup(HWND hWnd) {
    if (!isDragging || !currentProto) return;
    isDragging = false;
    ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    x2 = pt.x;
    y2 = std::max(pt.y, 32L);

    bool added = false;
    if (shapeCount < MY_SHAPE_ARRAY_SIZE) {
        if (currentToolId == ID_OBJ_POINT) {
            Shape* s = currentProto->Clone();
            s->Set(x2, y2, x2, y2);
            pcshape[shapeCount++] = s;
            added = true;
        } else if (x1 != x2 || y1 != y2) {
            Shape* s = currentProto->Clone();
            s->Set(x1, y1, x2, y2);
            pcshape[shapeCount++] = s;
            added = true;
        }
    }

    InvalidateRect(hWnd, nullptr, TRUE);
    if (added) {
        NotifyChanged();
    }
}

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    for (int i = 0; i < shapeCount; ++i) {
        if (pcshape[i]) {
            pcshape[i]->SetSelected(i == selectedIndex);
            pcshape[i]->Show(hdc);
        }
    }
    EndPaint(hWnd, &ps);
}

const Shape* MyEditor::GetShape(int index) const {
    if (index < 0 || index >= shapeCount) return nullptr;
    return pcshape[index];
}

void MyEditor::SelectShape(int index, HWND hWnd) {
    if (index < -1 || index >= shapeCount) {
        index = -1;
    }
    selectedIndex = index;
    InvalidateRect(hWnd, nullptr, TRUE);
}

void MyEditor::DeleteShape(int index, HWND hWnd) {
    if (index < 0 || index >= shapeCount) return;

    delete pcshape[index];
    for (int i = index; i < shapeCount - 1; ++i) {
        pcshape[i] = pcshape[i + 1];
    }
    pcshape[--shapeCount] = nullptr;
    selectedIndex = -1;

    InvalidateRect(hWnd, nullptr, TRUE);
    NotifyChanged();
}

void MyEditor::ClearAll(HWND hWnd) {
    FreeArray();
    selectedIndex = -1;
    InvalidateRect(hWnd, nullptr, TRUE);
    NotifyChanged();
}

bool MyEditor::SaveToFile(const std::wstring& filePath) const {
    std::ofstream out(std::filesystem::path(filePath), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return false;

    for (int i = 0; i < shapeCount; ++i) {
        if (!pcshape[i]) continue;
        std::string nameUtf8 = WideToUtf8(pcshape[i]->GetName());
        out << nameUtf8 << '\t'
            << pcshape[i]->GetX1() << '\t'
            << pcshape[i]->GetY1() << '\t'
            << pcshape[i]->GetX2() << '\t'
            << pcshape[i]->GetY2() << '\n';
    }
    return true;
}

bool MyEditor::LoadFromFile(const std::wstring& filePath, HWND hWnd) {
    std::ifstream in(std::filesystem::path(filePath), std::ios::binary);
    if (!in.is_open()) return false;

    FreeArray();
    selectedIndex = -1;

    std::string line;
    bool firstLine = true;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (firstLine && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }
        firstLine = false;
        if (line.empty()) continue;

        size_t tabPos = line.find('\t');
        if (tabPos == std::string::npos) continue;

        std::wstring name = Utf8ToWide(line.substr(0, tabPos));
        std::istringstream coords(line.substr(tabPos + 1));
        long lx1 = 0, ly1 = 0, lx2 = 0, ly2 = 0;
        if (!(coords >> lx1 >> ly1 >> lx2 >> ly2)) continue;

        if (shapeCount >= MY_SHAPE_ARRAY_SIZE) break;

        // Поліморфне створення фігури через реєстр прототипів (без прив'язки до конкретних класів)
        for (const Shape* proto : prototypes) {
            if (proto && name == proto->GetName()) {
                Shape* s = proto->Clone();
                s->Set(lx1, ly1, lx2, ly2);
                pcshape[shapeCount++] = s;
                break;
            }
        }
    }

    InvalidateRect(hWnd, nullptr, TRUE);
    NotifyChanged();
    return true;
}