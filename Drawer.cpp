// Drawer.cpp: файл реализации
//

#include "pch.h"
#include "Drawer.h"
#include<fstream>

// Drawer

IMPLEMENT_DYNAMIC(Drawer, CStatic)

Drawer::Drawer()
{
	GdiplusStartupInput si;
	GdiplusStartup(&token, &si, NULL);

	ifstream ifstr("DivergentColorMap.txt");
	for (int i = 0; i < 11; i++)
	{
		double r, g, b;
		ifstr >> r >> g >> b;
		Colors.push_back(m_Color(r, g, b));
	}
	ifstr.close();
}

Drawer::~Drawer()
{
	GdiplusShutdown(token);
}


BEGIN_MESSAGE_MAP(Drawer, CStatic)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// Обработчики сообщений Drawer



#include<thread>
void Drawer::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics ToWindow(lpDrawItemStruct->hDC);
	width = lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
	height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	Bitmap bmp(width, height);
	Graphics gr(&bmp);
	gr.Clear(Color::White);

	if (data.empty())
	{
		ToWindow.DrawImage(&bmp, 0, 0);
		return;
	}

	thread* linesthread = nullptr;
	bool joinable = false;
	if (!eqz)
	{
		linesthread = new thread([this]() {CreateLines(); });
		joinable = true;
	}
	gr.SetSmoothingMode(SmoothingModeAntiAlias);
	double actWidth(right - left);
	double actHeight(top - bot);
	double actLP = actWidth * lPadding / 100.;
	double actRP = actWidth * rPadding / 100.;
	double actTP = actHeight * tPadding / 100.;
	double actBP = actHeight * bPadding / 100.;

	double actTop = top + actTP;
	double actBot = bot - actBP;
	double actLeft = left - actLP;
	double actRight = right + actRP;

	actWidth = actRight - actLeft;
	actHeight = actTop - actBot;


	double xScale = width / actWidth;
	double yScale = -height / actHeight;

	Pen BackGroundPen(Color(200, 200, 200));

	Matrix matr;
	matr.Scale(xScale, yScale);
	matr.Translate(actLP - left, -actTop);


	float steplenY = (top - bot) / 10.;
	float steplenX = (right - left) / 10.;

	FontFamily FF(L"Arial");
	Gdiplus::Font font(&FF, 12, FontStyle::FontStyleRegular, UnitPixel);
	SolidBrush brush(Color::Black);
	for (int i = 0; i < 11; i++)
	{

		CString str;
		str.Format(L"%.2f", top - i * steplenY);

		PointF strPoint;
		strPoint.X = left - actWidth / 100 - CalcStringLen(lpDrawItemStruct->hDC, str) / xScale;
		strPoint.Y = top - i * steplenY -6 / yScale;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);

		str.Format(L"%.2f", right - i * steplenX);

		strPoint.X = right - i * steplenX - CalcStringLen(lpDrawItemStruct->hDC, str) / xScale / 2.;
		strPoint.Y = bot + 6 / yScale;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);
	}

	Pen pen(Color::Black, 1.1F);
	PointF p1(left, top), p2(left, bot);
	matr.TransformPoints(&p1);
	matr.TransformPoints(&p2);
	gr.DrawLine(&pen, p1, p2);

	p1 = PointF(left, bot);
	p2 = PointF(right, bot);
	matr.TransformPoints(&p1);
	matr.TransformPoints(&p2);
	gr.DrawLine(&pen, p1, p2);


	if (!title.IsEmpty())
	{

		p1.X = actLeft + actWidth / 2 - CalcStringLen(lpDrawItemStruct->hDC, title) / 2 / xScale;
		p1.Y = (actTop + top) / 2 - 6 / yScale;
		matr.TransformPoints(&p1);
		gr.DrawString(title, title.GetLength() + 1, &font, p1, &brush);
	}


	PointF ps[5];
	SolidBrush databrush(Color::White);
	Pen datapen(Color::White);
	for (int i = 0; i < top; i++)
	{
		for (int j = 0; j < right; j++)
		{
			Color clr = GetQuadreColor(i, j);
			databrush.SetColor(clr);
			datapen.SetColor(clr);
			ps[0].X = j;
			ps[0].Y = i;

			ps[1].X = j + 1;
			ps[1].Y = i;
			
			ps[2].X = j + 1;
			ps[2].Y = i + 1;

			ps[3].X = j;
			ps[3].Y = i + 1;

			matr.TransformPoints(ps, 4);
			gr.FillPolygon(&databrush, ps, 4);
			ps[4] = ps[0];
			gr.DrawLines(&datapen, ps, 5);
		}
	}

	double StepT = (outzmax - outzmin) / 10;
	PointF step;
	step.X = width * (100 - lPadding - rPadding) / 1100;
	step.Y = height * bPadding / 500.;
	ps[0].X = width * lPadding / 100. - step.X;
	ps[0].Y = height - 2 * step.Y;
	Pen blackpen(Color::Black);
	for (int i = 0; i < Colors.size(); i++)
	{
		databrush.SetColor(Colors[i].AsColor());
		ps[0].X += step.X;
		gr.FillRectangle(&databrush, ps[0].X, ps[0].Y, step.X, step.Y);
		gr.DrawRectangle(&blackpen, ps[0].X, ps[0].Y, step.X, step.Y);

		CString str;
		str.Format(L"%.1f", outzmin + i * StepT);
		double len = CalcStringLen(lpDrawItemStruct->hDC, str);
		PointF strp = ps[0];
		strp.X += (step.X - len) / 2;
		strp.Y += step.Y / 4;
		gr.DrawString(str, str.GetLength() + 1, &font, strp, &brush);
	}

	Pen pwpen(Color(218, 112, 214));
	for (auto& line : powerlines)
	{
		PointF p1(line[0].first, line[0].second);
		matr.TransformPoints(&p1);
		for (int i = 1; i < line.size(); i++)
		{
			PointF p2(line[i].first, line[i].second);
			matr.TransformPoints(&p2);
			gr.DrawLine(&pwpen, p1, p2);
			p1 = p2;
		}
	}


	if(joinable)
	linesthread->join();
	
	for (auto& line : lines)
	{
		PointF p1(line[0].first, line[0].second);
		matr.TransformPoints(&p1);
		for (int i = 1; i < line.size(); i++)
		{
			PointF p2(line[i].first, line[i].second);
			matr.TransformPoints(&p2);
			gr.DrawLine(&blackpen, p1, p2);
			p1 = p2;
		}
	}
	delete linesthread;
	/*
	//int alpha = 100;
	//SolidBrush GreyBrush(Color(alpha, 169, 169, 169));
	//Pen GreyPen(Color(alpha, 169, 169, 169));

	//ps[0].X = 0;
	//ps[0].Y = p.N - 1;
	//ps[1].X = p.ww;
	//ps[1].Y = 0;
	//matr.TransformPoints(ps, 2);
	////gr.DrawRectangle(&GreyPen, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));
	//gr.FillRectangle(&GreyBrush, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));

	//ps[0].X = p.ww;
	//ps[0].Y = p.ww;
	//ps[1].X = p.N - p.ww - 1;
	//ps[1].Y = 0;
	//matr.TransformPoints(ps, 2);
	////gr.DrawRectangle(&GreyPen, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));
	//gr.FillRectangle(&GreyBrush, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));

	//ps[0].X = p.N - p.ww - 1;
	//ps[0].Y = p.N - 1;
	//ps[1].X = p.N - 1;
	//ps[1].Y = 0;
	//matr.TransformPoints(ps, 2);
	////gr.DrawRectangle(&GreyPen, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));
	//gr.FillRectangle(&GreyBrush, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));

	//ps[0].X = p.ww;
	//ps[0].Y = p.N - 1;
	//ps[1].X = p.N - p.ww - 1;
	//ps[1].Y = p.N - p.ww - 1;
	//matr.TransformPoints(ps, 2);
	////gr.DrawRectangle(&GreyPen, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));
	//gr.FillRectangle(&GreyBrush, ps[0].X, ps[0].Y, distx(ps[0], ps[1]), disty(ps[0], ps[1]));
	*/
	ToWindow.DrawImage(&bmp, 0, 0);
}


void Drawer::SetData(std::vector<std::vector<double>>& in, ModelParams& p)
{
	if (in.empty())return;
	data = in;
	vector<double>Max;
	Max.resize(data.size());
	vector<double>Min = Max;
	for (int i = 0; i < data.size(); i++)
	{
		Max[i] = *max_element(data[i].begin(), data[i].end());
		Min[i] = *min_element(data[i].begin(), data[i].end());
	}
	zmax = *max_element(Max.begin(), Max.end());
	zmin = *min_element(Min.begin(), Min.end());
	if (zmax == zmin)
	{
		zmax++;
		eqz = true;
	}
	else eqz = false;

	outzmax = zmax;
	outzmin = zmin;
	
	right = data[0].size() - 1;
	left = 0;
	top = data.size() - 1;
	bot = 0;

	Normalize();
	MakeColorData();
	this->p = p;
}

void Drawer::Normalize()
{
	double Norm = max(right, top) / (zmax - zmin);
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			data[i][j] -= zmin;
			data[i][j] *= Norm;
		}
	}
	zmax = max(right, top);;
	zmin = 0;
}

void Drawer::SetPadding(double lP, double rP, double tP, double bP)
{
	lPadding = lP;
	rPadding = rP;
	tPadding = tP;
	bPadding = bP;
}
void Drawer::SetTitle(CString str)
{
	title = str;
}

void Drawer::SetLinesNum(int val)
{
	linesnum = val;
}

void Drawer::SetPowerlines(vector<vector<pair<double, double>>>& pl)
{
	powerlines = pl;
}


double Drawer::CalcStringLen(HDC hDC, CString str)
{
	double numlen = 0;
	int size = str.GetLength() + 1;
	for (int i = 0; i < size; i++)
	{
		ABCFLOAT abc;
		GetCharABCWidthsFloatW(hDC, str[i], str[i], &abc);
		numlen += abc.abcfA + abc.abcfB + abc.abcfC;
	}
	return numlen;
}


inline m_Color Drawer::GetColor(double val)
{
	double step = (zmax - zmin) / 9;
	double start = zmin;
	int i = 0;
	if (val > zmax)val = zmax;
	if (step != 0)
	{
		double ii = (val - zmin) / step;
		i = ii;
		if (ii - i > 0)i++;
	}
	m_Color res;
	double k = (zmin + step * i - val) / step;
	res.r = (1 - k) * Colors[i + 1].r + k * Colors[i].r;
	res.g = (1 - k) * Colors[i + 1].g + k * Colors[i].g;
	res.b = (1 - k) * Colors[i + 1].b + k * Colors[i].b;
	return res;
}

inline Color Drawer::GetQuadreColor(int i, int j)
{
	m_Color res = Colordata[i][j] + Colordata[i+1][j] + Colordata[i][j + 1] + Colordata[i + 1][j + 1];
	return res.AsColor();
}

inline void Drawer::MakeColorData()
{
	Colordata.resize(data.size());
	for (int i = 0; i < Colordata.size(); i++)
	{
		Colordata[i].resize(data[i].size());
		for (int j = 0; j < Colordata[i].size(); j++)
		{
			Colordata[i][j] = GetColor(data[i][j]);
		}
	}
}

pair<double, double> Drawer::CalcLinesDot(double val, int i1, int j1, int i2, int j2)
{
	double xc = max(j1,j2) - fabs(data[i2][j2] - val) * fabs(j2 - j1) / fabs(data[i2][j2] - data[i1][j1]);
	double yc = max(i1,i2) - fabs(data[i2][j2] - val) * fabs(i2 - i1) / fabs(data[i2][j2] - data[i1][j1]);
	return pair<double, double>(xc, yc);
}

void printlines(vector<vector<bool>>& data, ofstream&out)
{
	out << "\n\nNew Linedata:\n\n";
	out << setw(8) << setfill(' ') << ' ';
	for (int i = 0; i < data[0].size(); i++)out << setw(8) << setfill(' ') << i;
	out << endl;
	for (int i = 0; i < data.size(); i++)
	{
		out << setw(8) << setfill(' ') << i;
		for (int j = 0; j < data[i].size(); j++)
		{
			out << setw(8) << setfill(' ') << setprecision(1) << data[i][j];
		}
		out << endl;
	}
}
void Drawer::CreateLines()
{
	//ofstream ofstr("lines");
	lines.clear();
	double step = (zmax - zmin) / linesnum;
	double curval = 0;
	for (int k = 1; k < linesnum; k++)
	{
		curval = zmin + (0.5 + k) * step;
		vector<vector<bool>> linedata(data.size(), vector<bool>(data[0].size(), 0));
		int CellsCtr = 0;
		for (int i = 0; i < linedata.size() - 1; i++)
		{
			double b1 = 0;
			double b2 = 0;
			for (int j = 0; j < linedata[i].size() - 1; j++)
			{
				b1 = max(data[i][j], data[i][j + 1]);
				b2 = min(data[i][j], data[i][j + 1]);
				if ((curval <= b1) && (curval >= b2))
				{
					linedata[i][j] = true;
					CellsCtr++;
				}
				else
				{
					b1 = max(data[i][j], data[i + 1][j]);
					b2 = min(data[i][j], data[i + 1][j]);
					if ((curval <= b1) && (curval >= b2))
					{
						linedata[i][j] = true;
						CellsCtr++;
					}
					else
					{
						b1 = max(data[i + 1][j + 1], data[i][j + 1]);
						b2 = min(data[i + 1][j + 1], data[i][j + 1]);
						if ((curval <= b1) && (curval >= b2))
						{
							linedata[i][j] = true;
							CellsCtr++;
						}
						else
						{
							b1 = max(data[i + 1][j + 1], data[i + 1][j]);
							b2 = min(data[i + 1][j + 1], data[i + 1][j]);
							if ((curval <= b1) && (curval >= b2))
							{
								linedata[i][j] = true;
								CellsCtr++;
							}
						}
					}
				}
			}
		}

		//printlines(linedata, ofstr);
		while (CellsCtr > 1)
		{
			auto line = MakeLine(linedata, CellsCtr);
			if (line.size() > 1)
			{
				lines.push_back(TransformLine(line, curval));
			}
		}
	}
	//ofstr.close();
}

double Drawer::Distance(pair<double, double>& left, pair<double, double>& right)
{
	return (left.first - right.first) * (left.first - right.first) + (left.second - right.second) * (left.second - right.second);
}

vector<pair<double, double>> Drawer::MakeLine(vector<vector<bool>>& linedata, int& CellsCtr)
{
	vector<pair<double, double>>res;
	
	int bi = 0;
	int bj = 0;
	for (int j = 0; j < linedata[0].size(); j++)
	{
		bool bend = false;
		for (int i = 0; i < linedata.size(); i++)
		{
			if (linedata[i][j])
			{
				bend = true;
				bi = i;
				bj = j;
				break;
			}
		}
		if (bend)break;
	}


	int i = bi;
	int j = bj;
	while ((j >= 0) && (i >= 0) && (j < linedata[0].size()) && (i < linedata.size()))
	{
		res.push_back(pair<double, double>(j, i));
		linedata[i][j] = false;
		CellsCtr--;
		if ((j > 0) && (linedata[i][j - 1]))
		{
			j--;
			continue;
		}
		if ((j < linedata[0].size() - 1) && (linedata[i][j + 1]))
		{
			j++;
			continue;
		}
		if ((i > 0) && (linedata[i - 1][j]))
		{
			i--;
			continue;
		}
		if ((i < linedata.size() - 1) && (linedata[i + 1][j]))
		{
			i++;
			continue;
		}
		break;
	}
	return res;
}

bool forsort(pair<double, double>& left, pair<double, double>& right)
{
	return left.first < right.first;
}
vector<pair<double, double>> Drawer::TransformLine(vector<pair<double, double>>& ntline, double val)
{
	vector<pair<double, double>>res;
	pair<double, double> l, t, b, r;

	vector<pair<double, double>>temp, dist;
	
	if (FindIntersectionL(ntline[0].second, ntline[0].first, val, l))temp.push_back(l);
	if (FindIntersectionR(ntline[0].second, ntline[0].first, val, r))temp.push_back(r);
	if (FindIntersectionT(ntline[0].second, ntline[0].first, val, t))temp.push_back(t);
	if (FindIntersectionB(ntline[0].second, ntline[0].first, val, b))temp.push_back(b);

	if (temp[0].first < temp[1].first)
	{
		res.push_back(temp[0]);
		res.push_back(temp[1]);
	}
	else
	{
		res.push_back(temp[1]);
		res.push_back(temp[0]);
	}
	if (ntline.size() == 1)return res;

	for (int i = 1; i < ntline.size(); i++)
	{
		temp.clear();
		if (FindIntersectionL(ntline[i].second, ntline[i].first, val, l))temp.push_back(l);
		if (FindIntersectionR(ntline[i].second, ntline[i].first, val, r))temp.push_back(r);
		if (FindIntersectionT(ntline[i].second, ntline[i].first, val, t))temp.push_back(t);
		if (FindIntersectionB(ntline[i].second, ntline[i].first, val, b))temp.push_back(b);
		dist.clear();
		for (int j = 0; j < temp.size(); j++)
		{
			dist.push_back(pair<double, double>(Distance(temp[j], *res.rbegin()), j));
		}
		sort(dist.begin(), dist.end(), forsort);
		for (int j = 0; j < dist.size(); j++)
		{
			res.push_back(temp[dist[j].second]);
		}
	}
	return res;
}

bool Drawer::FindIntersectionT(int i, int j, double val, pair<double, double>& out)
{
	double b1 = max(data[i + 1][j + 1], data[i + 1][j]);
	double b2 = min(data[i + 1][j + 1], data[i + 1][j]);

	if (!((val <= b1) && (val >= b2)))return false;
	if (b1 == b2)
	{
		out.first = j;
		out.second = i;
		return true;
	}
	double xc = 0;
	double yc = 0;
	double x2 = 0;
	double x1 = 0;
	double T2 = 0;
	double T1 = 0;
	double y2 = 0;
	double y1 = 0;
	if (data[i + 1][j + 1] > data[i + 1][j])
	{
		x2 = j + 1;
		x1 = j;

		y2 = i + 1;
		y1 = i + 1;
		
		T2 = data[i + 1][j + 1];
		T1 = data[i + 1][j];
	}
	else
	{
		x1 = j + 1;
		x2 = j;

		y1 = i + 1;
		y2 = i + 1;

		T1 = data[i + 1][j + 1];
		T2 = data[i + 1][j];
	}
	xc = CalcXc(x1, x2, T1, T2, val);
	yc = CalcYc(y1, y2, T1, T2, val);
	out.first = xc;
	out.second = yc;
	return true;
}

bool Drawer::FindIntersectionB(int i, int j, double val, pair<double, double>& out)
{
	double b1 = max(data[i][j + 1], data[i][j]);
	double b2 = min(data[i][j + 1], data[i][j]);

	if (!((val <= b1) && (val >= b2)))return false;
	if (b1 == b2)
	{
		out.first = j;
		out.second = i;
		return true;
	}
	double xc = 0;
	double yc = 0;
	double x2 = 0;
	double x1 = 0;
	double T2 = 0;
	double T1 = 0;
	double y2 = 0;
	double y1 = 0;
	if (data[i][j + 1] > data[i][j])
	{
		x2 = j + 1;
		x1 = j;

		y2 = i;
		y1 = i;

		T2 = data[i][j + 1];
		T1 = data[i][j];
	}
	else
	{
		x1 = j + 1;
		x2 = j;

		y1 = i;
		y2 = i;

		T1 = data[i][j + 1];
		T2 = data[i][j];
	}
	xc = CalcXc(x1, x2, T1, T2, val);
	yc = CalcYc(y1, y2, T1, T2, val);
	out.first = xc;
	out.second = yc;
	return true;

}

bool Drawer::FindIntersectionL(int i, int j, double val, pair<double, double>& out)
{
	double b1 = max(data[i + 1][j], data[i][j]);
	double b2 = min(data[i + 1][j], data[i][j]);

	if (!((val <= b1) && (val >= b2)))return false;
	if (b1 == b2)
	{
		out.first = j;
		out.second = i;
		return true;
	}
	double xc = 0;
	double yc = 0;
	double x2 = 0;
	double x1 = 0;
	double T2 = 0;
	double T1 = 0;
	double y2 = 0;
	double y1 = 0;
	if (data[i + 1][j] > data[i][j])
	{
		x2 = j;
		x1 = j;

		y2 = i + 1;
		y1 = i;

		T2 = data[i + 1][j];
		T1 = data[i][j];
	}
	else
	{
		x1 = j;
		x2 = j;

		y1 = i + 1;
		y2 = i;

		T1 = data[i + 1][j];
		T2 = data[i][j];
	}
	xc = CalcXc(x1, x2, T1, T2, val);
	yc = CalcYc(y1, y2, T1, T2, val);
	out.first = xc;
	out.second = yc;
	return true;

}

bool Drawer::FindIntersectionR(int i, int j, double val, pair<double, double>& out)
{
	double b1 = max(data[i + 1][j + 1], data[i][j + 1]);
	double b2 = min(data[i + 1][j + 1], data[i][j + 1]);

	if (!((val <= b1) && (val >= b2)))return false;
	if (b1 == b2)
	{
		out.first = j;
		out.second = i;
		return true;
	}

	double xc = 0;
	double yc = 0;
	double x2 = 0;
	double x1 = 0;
	double T2 = 0;
	double T1 = 0;
	double y2 = 0;
	double y1 = 0;
	if (data[i + 1][j + 1] > data[i][j + 1])
	{
		x2 = j + 1;
		x1 = j + 1;

		y2 = i + 1;
		y1 = i;

		T2 = data[i + 1][j + 1];
		T1 = data[i][j + 1];
	}
	else
	{
		x1 = j + 1;
		x2 = j + 1;

		y1 = i + 1;
		y2 = i;

		T1 = data[i + 1][j + 1];
		T2 = data[i][j + 1];
	}
	xc = CalcXc(x1, x2, T1, T2, val);
	yc = CalcYc(y1, y2, T1, T2, val);
	out.first = xc;
	out.second = yc;
	return true;

}

double Drawer::CalcXc(double x1, double x2, double T1, double T2, double val)
{
	return x2 - (T2 - val) * (x2 - x1) / (T2 - T1);
}

double Drawer::CalcYc(double y1, double y2, double T1, double T2, double val)
{
	return y2 - (T2 - val) * (y2 - y1) / (T2 - T1);
}

inline double Drawer::distx(PointF p1, PointF p2)
{
	return fabs(p2.X - p1.X);
}

inline double Drawer::disty(PointF p1, PointF p2)
{
	return fabs(p2.Y - p1.Y);
}




void Drawer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	double actWidth(right - left);
	double actHeight(top - bot);
	double actLP = actWidth * lPadding / 100.;
	double actRP = actWidth * rPadding / 100.;
	double actTP = actHeight * tPadding / 100.;
	double actBP = actHeight * bPadding / 100.;

	double actTop = top + actTP;
	double actBot = bot - actBP;
	double actLeft = left - actLP;
	double actRight = right + actRP;

	actWidth = actRight - actLeft;
	actHeight = actTop - actBot;


	double xScale = width / actWidth;
	double yScale = -height / actHeight;

	Matrix matr;
	matr.Scale(xScale, yScale);
	matr.Translate(actLP - left, -actTop);

	matr.Invert();

	Clicked.X = point.x;
	Clicked.Y = point.y;
	matr.TransformPoints(&Clicked);

	parent->SendMessage(MS_POWERLINE);
	CStatic::OnLButtonUp(nFlags, point);
}
