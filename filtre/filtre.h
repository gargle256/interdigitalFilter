
// filtre.h�: fichier d'en-t�te principal pour l'application PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"		// symboles principaux


// CfiltreApp�:
// Consultez filtre.cpp pour l'impl�mentation de cette classe
//

class CfiltreApp : public CWinApp
{
public:
	CfiltreApp();

// Substitutions
public:
	virtual BOOL InitInstance();

// Impl�mentation

	DECLARE_MESSAGE_MAP()
};

extern CfiltreApp theApp;