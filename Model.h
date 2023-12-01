#pragma once
#include<vector>
#include<fstream>
#include<iomanip>
using namespace std;

enum CellType
{
	Metall,
	Border,
	Empty
};
enum ZubMode
{
	Agreed,
	AntiAgreed,
	NotAgreed
};
struct Cell
{
	CellType type = Empty;
	double fi = 0;
	Cell() {};
};

struct ModelParams
{
	int N = 0;

	double FiM = 0;
	double FiB = 0;

	int Zubl = 0;
	int Zubr = 0;
	int ZubLen = 0;

	ZubMode zmode = Agreed;

	int ww = 0;

	int ch = 0;
	int cw = 0;
	int cx = 0;
	int cy = 0;


	int bw = 0;

	double e = 0;
};
class Model
{
private:
	ModelParams p;
	vector<vector<Cell>>data;
	vector<vector<Cell>>prev;
	vector<vector<pair<double, double>>>powerlines;

	bool init = false;
protected:
	void Reset();
	void SetMeterials();
	void SetFi0();

	double CalcZubl(double i, double zlen);

	void PrintCellTypes();
	void PrintFi();

	double CalcDifference();
	
public:
	void Create(ModelParams&p);
	ModelParams GetParams();
	void main();
	vector<vector<double>> GetFi();
	void MakePowerline(double x, double y);
	vector<vector<pair<double, double>>>GetPowerlines();
	void ClearPowerlines();
};