// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "JDR.h"

#include "MainFrm.h"
#include "Board.h"
#include "BoardView.h"
#include "JDRDoc.h"
#include "JDRView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
/*	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
*/
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
/*
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
*/
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.cx = int(CBoard::m_width/SCREEN_RATIO)+20;
//	cs.cy = int(CBoard::m_height/SCREEN_RATIO)+200;
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	Sizing();
}

// CHILD 윈도우의 크기에 맞게 윈도우의 크기를 조정.
void CMainFrame::Sizing()
{
	CRect	rect;
	GetWindowRect(&rect);

	CRect	rectC;
	if(GetActiveView()==NULL)  {
		return;
	}
	GetActiveView()->GetClientRect(rectC);
//#define W (CBoard::m_width/SCREEN_RATIO+BOARD_VIEW_LEFT+BOARD_VIEW_LEFT)
//#define H (CBoard::m_height/SCREEN_RATIO+BOARD_VIEW_TOP)
#define W	JDR_VIEW_WIDTH
#define H 	JDR_VIEW_HEIGHT
	rect.right +=(W-rectC.right);
	rect.bottom+=(H-rectC.bottom);

	MoveWindow(&rect);

}

