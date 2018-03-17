
// filtreDlg.cpp : fichier d'impl�mentation
//

#include "stdafx.h"
#include "filtre.h"
#include "filtreDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// bo�te de dialogue CAboutDlg utilis�e pour la bo�te de dialogue '� propos de' pour votre application

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Donn�es de bo�te de dialogue
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Impl�mentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



CfiltreDlg::CfiltreDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILTRE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CfiltreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE, m_fileName);
	DDX_Control(pDX, IDC_EDIT_NUMBER, m_element);
	DDX_Control(pDX, IDC_EDIT_RIPPLE, m_ripple);
	DDX_Control(pDX, IDC_EDIT_CENTER, m_centerFreq);
	DDX_Control(pDX, IDC_EDIT_BW, m_BW);
	DDX_Control(pDX, IDC_EDIT_Z, m_impedance);
	DDX_Control(pDX, IDC_EDIT_GP, m_distGnd);
	DDX_Control(pDX, IDC_EDIT_ROD, m_diam);
	DDX_Control(pDX, IDC_EDIT_DIST_ROD, m_endSpacing);
	DDX_Control(pDX, IDC_EDIT_NB_PT, m_nbGraph);
	DDX_Control(pDX, IDC_EDIT_STEP, m_step);
}

BEGIN_MESSAGE_MAP(CfiltreDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CfiltreDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CfiltreDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// gestionnaires de messages pour CfiltreDlg

BOOL CfiltreDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// D�finir l'ic�ne de cette bo�te de dialogue.  L'infrastructure effectue cela automatiquement
	//  lorsque la fen�tre principale de l'application n'est pas une bo�te de dialogue
	SetIcon(m_hIcon, TRUE);			// D�finir une grande ic�ne
	SetIcon(m_hIcon, FALSE);		// D�finir une petite ic�ne

	ShowWindow(SW_MINIMIZE);

	return TRUE;  // retourne TRUE, sauf si vous avez d�fini le focus sur un contr�le
}

void CfiltreDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Si vous ajoutez un bouton R�duire � votre bo�te de dialogue, vous devez utiliser le code ci-dessous
//  pour dessiner l'ic�ne.  Pour les applications MFC utilisant le mod�le Document/Vue,
//  cela est fait automatiquement par l'infrastructure.

void CfiltreDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contexte de p�riph�rique pour la peinture

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrer l'ic�ne dans le rectangle client
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dessiner l'ic�ne
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Le syst�me appelle cette fonction pour obtenir le curseur � afficher lorsque l'utilisateur fait glisser
//  la fen�tre r�duite.
HCURSOR CfiltreDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfiltreDlg::OnBnClickedOk()
{
	CString temp;
	m_fileName.GetWindowTextW(temp);
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	CString path(currentDir);
	path += "\\";
	path += temp;
	m_filtre.setPath(path);


	m_element.GetWindowTextW(temp);
	int elem;
	swscanf_s(temp, L"%d", &elem);

	m_ripple.GetWindowTextW(temp);
	double ripple;
	swscanf_s(temp, L"%lf", &ripple);

	m_centerFreq.GetWindowTextW(temp);
	double centerFreq;
	swscanf_s(temp, L"%lf", &centerFreq);

	m_BW.GetWindowTextW(temp);
	double BW;
	swscanf_s(temp, L"%lf", &BW);

	m_impedance.GetWindowTextW(temp);
	double impedance;
	swscanf_s(temp, L"%lf", &impedance);

	m_distGnd.GetWindowTextW(temp);
	double distGnd;
	swscanf_s(temp, L"%lf", &distGnd);
	distGnd /= 2.54;

	m_diam.GetWindowTextW(temp);
	double diam;
	swscanf_s(temp, L"%lf", &diam);
	diam /= 2.54;

	m_endSpacing.GetWindowTextW(temp);
	double endSpacing;
	swscanf_s(temp, L"%lf", &endSpacing);
	endSpacing /= 2.54;

	m_nbGraph.GetWindowTextW(temp);
	int nbGraph;
	swscanf_s(temp, L"%d", &nbGraph);

	m_step.GetWindowTextW(temp);
	double step;
	swscanf_s(temp, L"%lf", &step);


	if ((elem < 2) || (elem > 12)) {
		m_element.SetFocus();
		m_element.SetSel(0,-1,FALSE);
		return;
	}
	if ((ripple < 0) || (ripple > 4)) {
		m_ripple.SetFocus();
		m_ripple.SetSel(0, -1, FALSE);
		return;
	}
	if (centerFreq<= 0) {
		m_centerFreq.SetFocus();
		m_centerFreq.SetSel(0, -1, FALSE);
		return;
	}
	if (BW <= 0) {
		m_BW.SetFocus();
		m_BW.SetSel(0, -1, FALSE);
		return;
	}
	if (impedance < 1) {
		m_impedance.SetFocus();
		m_impedance.SetSel(0, -1, FALSE);
		return;
	}
	if ((distGnd <= 0) || (distGnd <= diam)) {
		m_distGnd.SetFocus();
		m_distGnd.SetSel(0, -1, FALSE);
		return;
	}
	if (diam <= 0) {
		m_diam.SetFocus();
		m_diam.SetSel(0, -1, FALSE);
		return;
	}
	if (endSpacing <= (diam / 2)) {
		m_endSpacing.SetFocus();
		m_endSpacing.SetSel(0, -1, FALSE);
		return;
	}
	if (nbGraph < 3) {
		m_nbGraph.SetFocus();
		m_nbGraph.SetSel(0, -1, FALSE);
		return;
	}
	if (step <= 0) {
		m_step.SetFocus();
		m_step.SetSel(0, -1, FALSE);
		return;
	}

	m_filtre.run(elem, ripple, centerFreq, BW, impedance, distGnd, diam, endSpacing, nbGraph, step);
}


void CfiltreDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
