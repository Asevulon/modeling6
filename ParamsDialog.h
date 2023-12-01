#pragma once
#include "afxdialogex.h"


// Диалоговое окно ParamsDialog
#include"Model.h"
class ParamsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ParamsDialog)

public:
	ParamsDialog(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~ParamsDialog();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	int ww;
	double FiM;
	int cx;
	int cy;
	int cw;
	int ch;
	int zl;
	int zr;
	CComboBox zmode;
	int znl;
	double e;
	int bw;
	double Fib;
	int N;
	ModelParams p;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombo1();
	CEdit znlctr;
	CEdit rzctr;
};
