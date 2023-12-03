#include"pch.h"
#include"Model.h"

void Model::Reset()
{
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			data[i][j].type = Empty;
		}
	}
}

void Model::SetMeterials()
{
	data.resize(p.N);
	for (int i = 0; i < data.size(); i++)data[i].resize(p.N);

	int border = p.bw;
	//границы по краю
	for (int i = 0; i < p.N; i++)
	{
		for (int j = 0; j < border; j++)
		{
			data[i][j].type = Border;
			data[i][j].fi = p.FiB;
		}
		for (int j = p.N - border; j < p.N; j++)
		{
			data[i][j].type = Border;
			data[i][j].fi = p.FiB;
		}
	}
	
	for (int i = 0; i < p.bw; i++)
	{
		for (int j = p.bw; j < p.N - p.bw; j++)
		{
			data[i][j].type = Border;
			data[i][j].fi = p.FiB;
		}
	}

	for (int i = p.N - p.bw; i < p.N; i++)
	{
		for (int j = p.bw; j < p.N - p.bw; j++)
		{
			data[i][j].type = Border;
			data[i][j].fi = p.FiB;
		}
	}


	//стенки конденсатора
	for (int i = p.cy; i < p.cy + p.ch; i++)
	{
		for (int j = p.cx - p.ww; j < p.cx; j++)
		{
			data[i][j].type = Metall;
		}
	}

	for (int i = p.cy; i < p.cy + p.ch; i++)
	{
		for (int j = p.cx + p.cw; j < p.cx + p.cw + p.ww; j++)
		{
			data[i][j].type = Metall;
		}
	}

	//разобраться с зубчиками
	double zlen = double(p.ch) / double(p.Zubl);
	for (int i = p.cy; i < p.cy + p.ch ; i++)
	{
		double x = CalcZubl(i, zlen);
		for (int j = p.cx; j < p.cx + p.ZubLen; j++)
		{
			if (j < x + p.cx)data[i][j].type = Metall;
		}
	}

	if (p.zmode == Agreed)
	{
		for (int i = p.cy; i < p.cy + p.ch; i++)
		{
			for (int j = p.cx; j < p.cx + p.ZubLen; j++)
			{
				data[i][p.cx + p.cx + p.cw - j - 1] = data[i][j];
			}
		}
	}
	else
	{
		if (p.zmode == AntiAgreed)
		{
			for (int i = p.cy; i < p.cy + p.ch; i++)
			{
				for (int j = p.cx; j < p.cx + p.ZubLen; j++)
				{
					data[i][p.cx + p.cx + p.cw - j - 1] = data[p.cy + p.cy + p.ch - i - 1][j];
				}
			}
		}
		else
		{
			zlen = double(p.ch) / double(p.Zubr);
			for (int i = p.cy; i < p.cy + p.ch; i++)
			{
				double x = CalcZubl(i, zlen);
				for (int j = p.cx + p.cw - p.ZubLen; j < p.cx + p.cw; j++)
				{
					if (j > p.cx + p.cw - x  - 1)data[i][j].type = Metall;
				}
			}
		}
	}
}

void Model::SetFi0()
{
	for (int i = 0; i < p.N; i++)
	{
		for (int j = 0; j < p.N; j++)
		{
			if (data[i][j].type == Metall)data[i][j].fi = p.FiM;
			else if (data[i][j].type == Border)data[i][j].fi = p.FiB;
		}
	}
}

double Model::CalcZubl(double i, double zlen)
{
	return p.ZubLen * (i - p.cy - int((i - p.cy) / zlen) * zlen) / zlen;
}

void Model::PrintCellTypes()
{
	ofstream out("Cell Types");
	out << setw(8) << setfill(' ') << ' ';
	for (int i = 0; i < p.N; i++)out << setw(8) << setfill(' ') << i;
	out << endl;
	for (int i = 0; i < data.size(); i++)
	{
		out << setw(8) << setfill(' ') << i;
		for (int j = 0; j < data[i].size(); j++)
		{
			string str = "Empty";
			if (data[i][j].type == Metall)str = "Metall";
			else
				if (data[i][j].type == Border)str = "Border";

			out << setw(8) << setfill(' ') << str;
		}
		out << endl;
	}
	out.close();
}

void Model::PrintFi()
{
	ofstream out("Fi0");
	out << setw(8) << setfill(' ') << ' ';
	for (int i = 0; i < p.N; i++)out << setw(8) << setfill(' ') << i;
	out << endl;
	for (int i = 0; i < data.size(); i++)
	{
		out << setw(8) << setfill(' ') << i;
		for (int j = 0; j < data[i].size(); j++)
		{
			out << setw(8) << setfill(' ') <<setprecision(1)<< data[i][j].fi;
		}
		out << endl;
	}
	out.close();
}

double Model::CalcDifference()
{
	double res = 0;
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			res += (data[i][j].fi - prev[i][j].fi) * (data[i][j].fi - prev[i][j].fi);
		}
	}
	return sqrt(res / p.N);
}

double Model::Cos(double x, double y, double z, double A, double B, double C)
{
	double vmodule = sqrt(x * x + y * y + z * z);
	double nmodule = sqrt(A * A + B * B + C * C);
	double first = x * A + y * B + z * C;
	double second = vmodule * nmodule;
	return first / second;
}

void Model::Create(ModelParams& p)
{
	this->p = p;
	Reset();
	SetMeterials();

	SetFi0();

	init = true;
}

ModelParams Model::GetParams()
{
	return p;
}

void Model::main()
{
	if (!init)return;
	double ro = cos(atan(1) * 4 / p.N / p.N);
	//double w = 1. / (1. + ro * ro / 2.);
	double w = 1;
	do
	{
		prev = data;
		for (int i = 0; i < data.size(); i++)
		{
			for (int j = 0; j < data[i].size(); j++)
			{
				if (data[i][j].type == Empty)
				{
					data[i][j].fi = w * (data[i + 1][j].fi + data[i - 1][j].fi + data[i][j + 1].fi + data[i][j - 1].fi) / 4.;
				}
			}
		}
		//w = 1. / (1 + ro * ro * w / 4.);
	} while (CalcDifference() > p.e);
}

vector<vector<double>> Model::GetFi()
{
	vector<vector<double>>res;
	res.resize(data.size());
	for (int i = 0; i < data.size(); i++)
	{
		res[i].resize(data[i].size());
		for (int j = 0; j < data[i].size(); j++)
		{
			res[i][j] = data[i][j].fi;
		}
	}
	return res;
}

void Model::MakePowerline(double x, double y)
{
	if (!init)return;

	if ((x < 0) || (y < 0) || (x >= p.N) || (y >= p.N))return;

	int cap = p.N;
	int ctr = 0;
	double step = 0.3;
	
	int cxprev = -1;
	int cyprev = -1;


	vector<pair<double, double>>powerline;

	while (ctr < cap)
	{
		int cx = x;
		int cy = y;
		if ((cx != cxprev) || (cy != cyprev))
		{
			powerline.push_back(pair<double, double>(x, y));
			ctr++;
		}


		if ((cx < 0) || (cy < 0) || (cx >= p.N) || (cy >= p.N))break;


		double x1(cx), x2(0), x3(cx + 1),
			y1(cy), y2(0), y3(cy + 1),
			z1(data[cx][cy].fi), z2(0), z3(data[cx + 1][cy + 1].fi);
		
		if (y > x)
		{
			x2 = cx;
			y2 = cy + 1;
			z2 = data[cx][cy + 1].fi;
		}
		else
		{
			x2 = cx + 1;
			y2 = cy;
			z2 = data[cx + 1][cy].fi;
		}

		double A = (y2 - y1) * (z3 - z1) - (y3 - y1) * (z2 - z1);
		double B = (x2 - x1) * (z3 - z1) - (x3 - x1) * (z2 - z1);
		double C = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);

		//double mcos = Cos(x2 - x1, y2 - y1, z2 - z1, A, B, C);
		if (C > 0)
		{
			A = -A;
			B = -B;
		}

		double gradabs = sqrt(A * A + B * B);
		A /= gradabs;
		B /= gradabs;

		x -= step * A;
		y -= step * B;

		cxprev = cx;
		cyprev = cy;
		
	}

	powerlines.push_back(powerline);
}

vector<vector<pair<double, double>>> Model::GetPowerlines()
{
	return powerlines;
}

void Model::ClearPowerlines()
{
	powerlines.clear();
}

void Model::MakeMultiplePowerlines(int num)
{
	ClearPowerlines();
	if (num < 3)return;

	double step = p.N / (num - 1.);

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			double x = j * step;
			double y = i * step;
			MakePowerline(x, y);
		}
	}
}
