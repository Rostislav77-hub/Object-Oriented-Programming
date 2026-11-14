#pragma once
#include <windows.h>

class Shape {
protected:
    long xs1 = 0, ys1 = 0, xs2 = 0, ys2 = 0;
    bool isRubber = false;

    HPEN CreateCurrentPen() const;

public:
    virtual ~Shape() = default;
    void Set(long x1, long y1, long x2, long y2);

    virtual void Show(HDC hdc) = 0;
    virtual void ShowRubber(HDC hdc);
    virtual Shape* Clone() const = 0;
};

class PointShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new PointShape(*this); }
};

class LineShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineShape(*this); }
};

class RectShape : virtual public Shape {
protected:
    bool useOrangeFill = true;
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new RectShape(*this); }
};

class EllipseShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new EllipseShape(*this); }
};

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineOOShape(*this); }
};

class CubeShape : public RectShape, public LineShape {
public:
    CubeShape();
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new CubeShape(*this); }
};