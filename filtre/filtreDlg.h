
// filtreDlg.h : fichier d'en-t�te
//

#pragma once
#include "afxwin.h"
#include "calculFiltre.h"



// bo�te de dialogue CfiltreDlg
class CfiltreDlg : public CDialogEx
{
// Construction
public:
	CfiltreDlg(CWnd* pParent = NULL);	// constructeur standard

// Donn�es de bo�te de dialogue
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTRE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Prise en charge de DDX/DDV


// Impl�mentation
protected:
	HICON m_hIcon;

	// Fonctions g�n�r�es de la table des messages
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	CEdit m_fileName;
	calculFiltre m_filtre;
	CEdit m_element;
	CEdit m_ripple;
	CEdit m_centerFreq;
	CEdit m_BW;
	CEdit m_impedance;
	CEdit m_distGnd;
	CEdit m_diam;
	CEdit m_endSpacing;
	CEdit m_nbGraph;
	CEdit m_step;
};
