
// MyDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "modeling6.h"
#include "MyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно MyDlg



MyDlg::MyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODELING6_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODEL_PICTURE, drw);
	DDX_Control(pDX, IDC_LINES_SLIDER, LinesSlider);
	DDX_Control(pDX, IDC_POWERLINES_SLIDER, PLSlider);
}

BEGIN_MESSAGE_MAP(MyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &MyDlg::OnBnClickedOk)
	ON_WM_HSCROLL()
	ON_MESSAGE(MS_POWERLINE, &MyDlg::OnMsPowerline)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &MyDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// Обработчики сообщений MyDlg

BOOL MyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	LinesSlider.SetRange(3, 25);
	LinesSlider.SetPos(6);
	drw.SetLinesNum(LinesSlider.GetPos());

	PLSlider.SetRange(2, 75);
	PLSlider.SetPos(2);

	drw.parent = this;
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void MyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR MyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT MyDlg::OnMsPowerline(WPARAM wParam, LPARAM lParam)
{
	m.MakePowerline(drw.Clicked.X, drw.Clicked.Y);
	drw.SetPowerlines(m.GetPowerlines());
	drw.Invalidate();

	return LRESULT();
}



void MyDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (pdlg.DoModal() != IDOK)return;

	m.Create(pdlg.p);
	m.main();
	drw.SetData(m.GetFi(), m.GetParams());
	drw.Invalidate();
}




void MyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	drw.SetLinesNum(LinesSlider.GetPos());
	m.MakeMultiplePowerlines(PLSlider.GetPos());
	drw.SetPowerlines(m.GetPowerlines());
	drw.Invalidate();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void MyDlg::OnBnClickedButtonClear()
{
	// TODO: добавьте свой код обработчика уведомлений
	m.ClearPowerlines();
	drw.SetPowerlines(m.GetPowerlines());
	drw.Invalidate();
}
