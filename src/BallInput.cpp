// BallInput.cpp : implementation file
//

#include "stdafx.h"
#include "JDR.h"
#include "BallInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Ball.h"
#define BALLINPUT_WIDTH		((int)BALL_DIAMETER)
#define BALLINPUT_HEIGHT	((int)BALL_DIAMETER)


/////////////////////////////////////////////////////////////////////////////
// CBallInput

CBallInput::CBallInput()
{
	m_x = BALL_DIAMETER/2;
	m_y = BALL_DIAMETER/2;
}

CBallInput::~CBallInput()
{
}


BEGIN_MESSAGE_MAP(CBallInput, CWnd)
	//{{AFX_MSG_MAP(CBallInput)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBallInput message handlers

bool CBallInput::CreateWnd(CWnd * pParent, int x, int y)
{
	ASSERT(pParent);
	LPCTSTR lpszWindowName = "BallInput";
	LPCTSTR	lpszClassName;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;//|WS_THICKFRAME;
	lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		LoadCursor(NULL,IDC_ARROW),
//		(HBRUSH)(::GetStockObject(LTGRAY_BRUSH)),
		(HBRUSH)(::GetStockObject(WHITE_BRUSH)),
		NULL);

	VERIFY(CWnd::CreateEx(WS_EX_TOPMOST,lpszClassName,lpszWindowName,
		dwStyle,x,y,BALLINPUT_WIDTH,BALLINPUT_HEIGHT,*pParent,NULL,NULL));

	return true;
}

void CBallInput::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);
	m_x = point.x;
	m_y = point.y;
	Invalidate(TRUE);

}

void CBallInput::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	this->GetWindowRect(&rect);
	dc.Ellipse(0,0,BALLINPUT_WIDTH,BALLINPUT_HEIGHT);

	dc.MoveTo(int(m_x-2),int(m_y));
	dc.LineTo(int(m_x+3),int(m_y));
	dc.MoveTo(int(m_x),int(m_y-2));
	dc.LineTo(int(m_x),int(m_y+3));

}

CPoint CBallInput::GetXY()
{
	return CPoint(int(m_x),int(m_y));
}

void CBallInput::GetXY(double & x, double & y)
{
	x = (m_x-BALL_DIAMETER/2);
	y = -(m_y-BALL_DIAMETER/2);
}

