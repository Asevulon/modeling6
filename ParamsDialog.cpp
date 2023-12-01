// ParamsDialog.cpp: файл реализации
//

#include "pch.h"
#include "modeling6.h"
#include "afxdialogex.h"
#include "ParamsDialog.h"


// Диалоговое окно ParamsDialog

IMPLEMENT_DYNAMIC(ParamsDialog, CDialogEx)

ParamsDialog::ParamsDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARAM_DIALOG, pParent)
	, ww(2)
	, FiM(2)
	, cx(30)
	, cy(40)
	, cw(40)
	, ch(20)
	, zl(5)
	, zr(1)
	, znl(5)
	, e(1e-3)
	, bw(4)
	, Fib(1)
	, N(100)
{

}

ParamsDialog::~ParamsDialog()
{
}

void ParamsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT4, ww);
	DDX_Text(pDX, IDC_EDIT5, FiM);
	DDX_Text(pDX, IDC_EDIT6, cx);
	DDX_Text(pDX, IDC_EDIT7, cy);
	DDX_Text(pDX, IDC_EDIT8, cw);
	DDX_Text(pDX, IDC_EDIT9, ch);
	DDX_Text(pDX, IDC_EDIT10, zl);
	DDX_Text(pDX, IDC_EDIT11, zr);
	DDX_Control(pDX, IDC_COMBO1, zmode);
	DDX_Text(pDX, IDC_EDIT13, znl);
	DDX_Text(pDX, IDC_EDIT1, e);
	DDX_Text(pDX, IDC_EDIT2, bw);
	DDX_Text(pDX, IDC_EDIT3, Fib);
	DDX_Text(pDX, IDC_EDIT14, N);
	DDX_Control(pDX, IDC_EDIT13, znlctr);
	DDX_Control(pDX, IDC_EDIT11, rzctr);
}


BEGIN_MESSAGE_MAP(ParamsDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &ParamsDialog::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &ParamsDialog::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// Обработчики сообщений ParamsDialog


BOOL ParamsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Добавить дополнительную инициализацию
	zmode.InsertString(-1, L"Согласованные");
	zmode.InsertString(-1, L"Антисогласованные");
	zmode.InsertString(-1, L"Не согласованные");
	zmode.SetCurSel(p.zmode);
	CString str;
	zmode.GetLBText(zmode.GetCurSel(), str);
	if ((str == L"Согласованные") || (str == L"Антисогласованные"))rzctr.EnableWindow(FALSE);
	else if (str == L"Не согласованные")rzctr.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// Исключение: страница свойств OCX должна возвращать значение FALSE
}


void ParamsDialog::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData();

	p.N = N;

	p.bw = bw;
	p.FiB = Fib;

	p.FiM = FiM;
	p.cx = cx;
	p.cy = cy;
	p.ch = ch;
	p.cw = cw;
	p.ww = ww;

	p.Zubl = znl;
	p.Zubr = zr;
	p.ZubLen = zl;

	p.e = e;
	
	CString str;
	zmode.GetLBText(zmode.GetCurSel(), str);
	if (str == L"Согласованные")p.zmode = Agreed;
	else if (str == L"Антисогласованные")p.zmode = AntiAgreed;
	else if (str == L"Не согласованные")p.zmode = NotAgreed;

	CDialogEx::OnOK();
}


void ParamsDialog::OnCbnSelchangeCombo1()
{
	// TODO: добавьте свой код обработчика уведомлений
	CString str;
	zmode.GetLBText(zmode.GetCurSel(), str);
	if ((str == L"Согласованные") || (str == L"Антисогласованные"))rzctr.EnableWindow(FALSE);
	else if (str == L"Не согласованные")rzctr.EnableWindow(TRUE);
}
