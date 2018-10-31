// ControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JDR.h"
#include "ControlDlg.h"
#include <math.h>
#include "BoardView.h"
#include "Util.h"
#include "BallInput.h"
#include "Ball.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlDlg dialog


CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDlg::IDD, pParent)
{
	m_pParent = (CBoardView*) pParent;
	m_nBallNum = 0;

	//{{AFX_DATA_INIT(CControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlDlg)
	DDX_Control(pDX, IDC_SLIDER4, m_sldPower);
//	DDX_Control(pDX, IDC_SLIDER3, m_sldDx);
//	DDX_Control(pDX, IDC_SLIDER2, m_sldBy);
//	DDX_Control(pDX, IDC_SLIDER1, m_sldBx);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CControlDlg, CDialog)
	//{{AFX_MSG_MAP(CControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
//	ON_STN_CLICKED(IDC_STATIC,OnStaticClick)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlDlg message handlers

void CControlDlg::OnButtonTest() 
{
	m_dPower = m_sldPower.GetPos();

	m_pBallInputWnd->GetXY(m_dBx,m_dBy);

//	m_dDx = cos(m_sldDx.GetPos()*3.14/180);
//	m_dDy = sin(m_sldDx.GetPos()*3.14/180);

	m_pParent->Stroke(
		m_nBallNum,
		m_dBx,m_dBy,
//		m_dDx,m_dDy,
		m_dPower);
}

void CControlDlg::OnStaticClick(WPARAM wParam,LPARAM lParam)
{
	m_dBx = LOWORD(lParam);
	m_dBy = HIWORD(lParam);
	CString str="??";
	str.Format("x: %f y:%f ",m_dBx,m_dBy);
	MessageBox(str);
	m_pParent->Stroke(0,0,0,1,1,3000);
}

int CControlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CControlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pBallInputWnd = new CBallInput;
	m_pBallInputWnd->CreateWnd(this,20,20);

	m_sldPower.SetRangeMin(0);
	m_sldPower.SetRangeMax(MAX_BALL_V0_SPEED);		// 최고 초속도 지정
//	m_sldDx.SetRangeMax(360);
//	m_sldBy.SetRangeMax((int)BALL_DIAMETER);
//	m_sldBx.SetRangeMax((int)BALL_DIAMETER);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CControlDlg::OnClose() 
{
	CDialog::OnClose();
}

void CControlDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

CControlDlg::~CControlDlg()
{
	delete m_pBallInputWnd;
}
