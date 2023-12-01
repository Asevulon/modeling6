
// modeling6.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// Cmodeling6App:
// Сведения о реализации этого класса: modeling6.cpp
//

class Cmodeling6App : public CWinApp
{
public:
	Cmodeling6App();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern Cmodeling6App theApp;
