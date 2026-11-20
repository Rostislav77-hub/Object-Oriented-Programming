#pragma once
#include <windows.h>

class Shape {
protected:
    long xs1 = 0, ys1 = 0, xs2 = 0, ys2 = 0;
    bool isRubber = false;
    bool isSelected = false;

    HPEN CreateCurrentPen() const;

public:
    virtual ~Shape() = default;

    void Set(long x1, long y1, long x2, long y2);
    long GetX1() const { return xs1; }
    long GetY1() const { return ys1; }
    long GetX2() const { return xs2; }
    long GetY2() const { return ys2; }

    void SetSelected(bool selected) { isSelected = selected; }
    bool IsSelected() const { return isSelected; }

    virtual void Show(HDC hdc) = 0;
    void ShowRubber(HDC hdc);
    virtual Shape* Clone() const = 0;
    virtual const wchar_t* GetName() const = 0;
};

class PointShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new PointShape(*this); }
    const wchar_t* GetName() const override { return L"Крапка"; }
};

class LineShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineShape(*this); }
    const wchar_t* GetName() const override { return L"Лінія"; }
};

class RectShape : virtual public Shape {
protected:
    bool useOrangeFill = true;
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new RectShape(*this); }
    const wchar_t* GetName() const override { return L"Прямокутник"; }
};

class EllipseShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new EllipseShape(*this); }
    const wchar_t* GetName() const override { return L"Еліпс"; }
};

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineOOShape(*this); }
    const wchar_t* GetName() const override { return L"Лінія з кружечками"; }
};

class CubeShape : public RectShape, public LineShape {
public:
    CubeShape() { useOrangeFill = false; }
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new CubeShape(*this); }
    const wchar_t* GetName() const override { return L"Каркас куба"; }
};