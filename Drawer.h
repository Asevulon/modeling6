#pragma once


// Drawer
#include"Model.h"
#include<vector>
#include<algorithm>
using namespace std;
using namespace Gdiplus;


#define MS_POWERLINE WM_USER + 1


struct  m_Color
{
	double r = 0; double g = 0; double b = 0;
	m_Color() {}
	m_Color(double r, double g, double b) :r(r), g(g), b(b) {}
	Gdiplus::Color AsColor() { return Gdiplus::Color(r, g, b); }
	m_Color operator+ (const m_Color& right) { m_Color res = *this; res.r += right.r; res.g += right.g; res.b += right.b; res.r /= 2; res.g /= 2; res.b /= 2; return res; }
	void ToOpenGlRange() { r /= 255; g /= 255; b /= 255; }
};


class Drawer : public CStatic
{
	DECLARE_DYNAMIC(Drawer)
private:
	vector<vector<double>>data;
	double lPadding = 10;
	double rPadding = 5;
	double tPadding = 5;
	double bPadding = 22;

	int width;
	int height;

	double left;
	double right;
	double bot;
	double top;
	double zmax;
	double zmin;
	double outzmax;
	double outzmin;
	ULONG_PTR token;
	
	CString title;

	
	std::vector<m_Color>Colors;
	vector<vector<m_Color>>Colordata;

	vector < vector<pair<double, double>>> lines, powerlines;
	bool eqz = false;

	int linesnum = 6;

	ModelParams p;
public:
	Drawer();
	virtual ~Drawer();

protected:
	
	double CalcStringLen(HDC hDC, CString string);
	void Drawer::Normalize();
	inline m_Color Drawer::GetColor(double val);
	inline Color Drawer::GetQuadreColor(int i, int j);
	inline void MakeColorData();
	pair<double, double>CalcLinesDot(double val, int i1, int j1, int i2, int j2);
	void CreateLines();
	double Distance(pair<double, double>& left, pair<double, double>& right);
	vector<pair<double, double>>MakeLine(vector<vector<bool>>& linedata, int &CellsCtr);
	vector<pair<double, double>>TransformLine(vector<pair<double, double>>&ntline, double val);
	bool FindIntersectionT(int i, int j, double val, pair<double, double>& out);
	bool FindIntersectionB(int i, int j, double val, pair<double, double>& out);
	bool FindIntersectionL(int i, int j, double val, pair<double, double>& out);
	bool FindIntersectionR(int i, int j, double val, pair<double, double>& out);
	double CalcXc(double x1, double x2, double T1, double T2, double val);
	double CalcYc(double y1, double y2, double T1, double T2, double val);

	inline double distx(PointF p1, PointF p2);
	inline double disty(PointF p1, PointF p2);
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void Drawer::SetData(std::vector<std::vector<double>>& in, ModelParams& p);

	//Отступ задается в процентах (диапазон значений от 0 до 100)
	void SetPadding(double lPadding, double rPadding, double tPadding, double bPadding);
	void SetTitle(CString str);
	void SetLinesNum(int val);
	void SetPowerlines(vector<vector<pair<double, double>>>& pl);
	CWnd* parent;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	PointF Clicked;
};


