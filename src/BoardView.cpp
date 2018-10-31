// BoardView.cpp : implementation file
//

#include "stdafx.h"
#include "JDR.h"
#include "BoardView.h"
#include "Util.h"
#include "mmsystem.h"
#include <math.h>
#include "JDRDoc.h"
#include "JDRView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _ON_IDLE_			// ȭ���� ���� ������ �����Ѵ�.
							// define �Ǿ� ������ CWinApp�� OnIdle�� ȭ���� �����ϰ�
							// �ƴϸ� MMTimer�� �����Ѵ�.

#define BALL_DIAMETER		65.5
#define BALL_VIEW_DIAMETER	(BALL_DIAMETER/SCREEN_RATIO)


#define TIMER_ID_SCREEN_REFRESH		1		// ȭ�� ���ſ�.
#define TIMER_ID_DO_PROCESS			2		// ���� ó����.
#define TIMER_ID_BALL_CHECK			3		// ���� ���� üũ��
#define SCREEN_FREFRESH_GAP			20		// 30ms ���� ȭ�� ����.


/////////////////////////////////////////////////////////////////////////////
// CBoardView

HWND	g_hWnd=NULL;
bool	g_bThreadOn = true;
UINT	Ticker(LPVOID	pParam) 
{
	int prevTick = GetTickCount();
	int curTick;
	for(;;) {
		curTick = GetTickCount();
		if(prevTick+TIME_GAP<curTick) {
			if(g_hWnd)
			  PostMessage((HWND)g_hWnd,WM_TIMER,TIMER_ID_DO_PROCESS,0);
			prevTick += TIME_GAP;
		}
		Sleep(1);
		if(g_bThreadOn==false) break;
	}
	return 0;
}


/* MMTimer �� �޾Ƽ� ���� ���� WM_TIMER�� �����쿡 ������. */
void CALLBACK CatchMMTimer(UINT IDEvent,      /* event Id */
                           UINT uReserved,    /* unused   */
                           DWORD dwUser,      /* our hWnd */
                           DWORD dwReserved1, /* unused   */
                           DWORD dwReserved2) /* unused   */
{
//    PostMessage((HWND)dwUser,WM_TIMER,TIMER_ID_DO_PROCESS,0);
}

CBoardView::CBoardView()
{
	srand( (unsigned)time( NULL ) );

	m_dx = -10;
	m_dy = -10;
	m_bCaptured = false;
	m_bLeftCaptured = false;

	m_pRedBrush = new CBrush(RGB(255,0,0));
	m_pWhiteBrush = new CBrush(RGB(255,255,255));
	m_pGreenBrush = new CBrush(RGB(0,176,0));

	m_nStartClock = GetTickCount();
	m_nRefreshCount = 0;
	m_nStartClock = 0;

	m_nLastPlayer = 0;
	m_bMovingNow = false;
	m_nFirstPlayersScore = 0;
	m_nSecondPlayersScore = 0;

	m_pParent = NULL;

	m_ai30.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_ai30.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

	m_ai50.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_ai50.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

	m_ai80.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_ai80.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

	m_ai100.SetParent(this);
	m_ai100.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_ai100.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

	m_ai150.SetParent(this);
	m_ai150.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_ai150.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

	m_aiBpn.SetBoard(&m_board); // ������ �����͸� �ѱ��.
	m_aiBpn.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// ���� ���̸��� �ݴ���� �����Ѵ�.

}

CBoardView::~CBoardView()
{
//debug("~BoardView()");
	// END THREAD ( Ticker() )
	g_bThreadOn = false;

	delete m_pRedBrush;
	delete m_pWhiteBrush;
	delete m_pGreenBrush;

	debug_end();
}


BEGIN_MESSAGE_MAP(CBoardView, CWnd)
	//{{AFX_MSG_MAP(CBoardView)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBoardView message handlers

/* Parent Window �� �޾Ƽ� Window�� ���� �Ѵ�.*/
bool CBoardView::CreateWin(CWnd * pParent, int top = BOARD_VIEW_TOP, int left = BOARD_VIEW_LEFT)
{
	ASSERT(pParent);
	LPCTSTR lpszWindowName = "BillBoard";
	LPCTSTR	lpszClassName;
//	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_THICKFRAME;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
	lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		LoadCursor(NULL,IDC_ARROW),
		(HBRUSH)(::GetStockObject(LTGRAY_BRUSH)),
		NULL);
	VERIFY(CWnd::CreateEx(WS_EX_TOPMOST,lpszClassName,lpszWindowName,
		dwStyle,left,top,
		BOARD_VIEW_WIDTH,BOARD_VIEW_HEIGHT,*pParent,NULL,NULL));

	m_pParent = (CJDRView*)pParent;

	// CONTROL DIALOG ����
/*
	m_pDlg = new CControlDlg(this);
	((CDialog*)m_pDlg)->Create(CControlDlg::IDD,this);
	m_pDlg->ShowWindow(SW_SHOW);
*/

	// LOAD BACKGROUND IMAGE ( or Create It )
	CRect	rect;
	CDC *pDC=GetDC();
	GetClientRect(&rect);
	m_bmBack.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());
	m_bmBackGnd.LoadBitmap(IDB_BOARD);     // bitmap �о� ���̱�.

	// �޸� DC ����.
	m_dcMEM.CreateCompatibleDC(pDC);
	m_dcMEMBackGnd.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);
	m_pOldBitmap = m_dcMEM.SelectObject(&m_bmBack);
	m_pOldBitmapBackGnd = m_dcMEMBackGnd.SelectObject(&m_bmBackGnd);

	// �޸� DC�� ���ʷ� �׸���
	DrawBall2MemDC();

	// SET MM TIMER.
	SetMMTimer();

	// SET SCREEN REFRESH TIMER
	SetTimer(TIMER_ID_SCREEN_REFRESH,SCREEN_FREFRESH_GAP,NULL);
	SetTimer(TIMER_ID_BALL_CHECK,100,NULL);


	// M THREAD
	g_hWnd = this->m_hWnd;
	AfxBeginThread(Ticker,NULL,THREAD_PRIORITY_NORMAL);


	return true;
}
// �ܼ��� �޸� DC�� ȭ�鿡 �ѷ� ������.
void CBoardView::OnPaint() 
{
	CPaintDC dc(this); 

	// DRAW TO SCREEN 
	dc.BitBlt(0,0,BOARD_VIEW_WIDTH,BOARD_VIEW_HEIGHT,
		&m_dcMEM,0,0,SRCCOPY);
}

// �� �Ѹ���. (�����ϰ�)
void CBoardView::Test()
{
	m_board.m_nBallHitCount = 0;
	for(int i=0;i<BALL_NUM;i++) 
		m_board.GetBall(i)->SetVXY(rand()%MAX_BALL_V0_SPEED*2-MAX_BALL_V0_SPEED,
			rand()%MAX_BALL_V0_SPEED*2-MAX_BALL_V0_SPEED);
}
// Timer �� �޾Ƽ� ���� �����̰� ȭ�鵵 �����Ѵ�.
void CBoardView::OnTimer(UINT nIDEvent) 
{
	static int count=0;

	switch(nIDEvent) 
	{
		case TIMER_ID_BALL_CHECK :
				if(m_board.IsStop()&&m_bMovingNow==true) 
				{
					debug("BallStop");
					m_board.GetResult();
					int eval = m_board.GetEvalResult();
//debug("�����÷��̾�[%d]�� �����򰡰��:%d",m_nLastPlayer,eval);
					{
						CString	str;
						int		&nCurScore = (m_nLastPlayer%2==0) ? m_nFirstPlayersScore  : m_nSecondPlayersScore;
						str.Format("���� ���[%d]�� ",m_nLastPlayer);
						if(eval>=EVAL_GOOD) 
						{
							str+= "������ �÷Ƚ��ϴ�.";
							nCurScore-=10;
						}
						else if(eval>=EVAL_NOT_BAD) 
						{
							str+= "�������� ���߽��ϴ�.";
						}
						else 
						{
							str+= "������ �ϼ̽��ϴ�.";
							nCurScore+=10;
						}
						msg(str);
						str.Format("[ ���� : %d ]",nCurScore);
						msg(str);
						msg("---------------------------------------------------------------");
					}
					m_board.ClearBallEvent();
//DoAI50();									// BPN�� ����Ÿ �����.
//if(eval>=10000) SaveLastData();				// �� ����� �������� ���Ϸ� ����.
				}
				m_bMovingNow = !m_board.IsStop();
				break;
	case TIMER_ID_SCREEN_REFRESH :
				#ifndef _ON_IDLE_
				#ifndef NDEBUG
						DrawBall2MemDC();
						Invalidate(FALSE);
				#endif
				#endif
				break;
	case TIMER_ID_DO_PROCESS :
				#ifndef _ON_IDLE_
				#ifdef NDEBUG
						if(count++==5) {	// ȭ�� ����
							count = 0;
							DrawBall2MemDC();
							DrawMemDC2Screen();
						}
				#endif
				#endif
						m_board.DoStep();	// �� �����̱�.
						break;
	}
}

// ��Ʈ�� ���̾˷α� �����ֱ�.
void CBoardView::Test2()
{
//	m_pDlg->ShowWindow(SW_SHOW);
}

void CBoardView::OnDestroy() 
{
//debug("OnDestroy");

	DisableMMTimer();

	// MEMORY FREE
//	m_pDlg->DestroyWindow();
//	delete m_pDlg;

    m_dcMEM.SelectObject(m_pOldBitmap);
    m_dcMEMBackGnd.SelectObject(m_pOldBitmapBackGnd);
	CWnd::OnDestroy();
}


// ControlDlg���� ȣ���Ѵ�. ==> CJDRView���� ȣ��� �����.
bool CBoardView::Stroke(int n, double bx, double by, double power)
{
if(m_bMovingNow) 
	debug("����-���� �����̴µ� �ǵ����..");


	double dx, dy;
	// ��� �б�
//	m_board.GetBall(0)->ReadConstant();

	// ���� ��ġ
	double x = m_board.GetBall(n)->GetX();
	double y = m_board.GetBall(n)->GetY();

	// ���� �������� �Ÿ�
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );

	// dx, �� dy �� ControlDialog���� �Է¹��� �ʰ� ���� ó���Ѵ�.
	// dx �� dy ���  * dx*dx+dy*dy=1 �̴�.!!
	dx = (m_dx-x)/d;
	dy = (m_dy-y)/d;

	GetCurData(n,dx,dy,bx,by,power);

	m_bMovingNow = true;			// ���� �����̱� �����ߴ�...
	m_nLastPlayer = n;				// ���������� ģ��� .
	m_board.SetCurPlayer(m_nLastPlayer%2);		// ¦����, Ȧ����.

	PlayResource("IDR_WAVE_STROKE");				// ť��� ���� ĥ�� �Ҹ�

	return m_board.GetBall(n)->Stroke(bx,by,dx,dy,power);
}


// AI���� ����Ѵ�.
bool CBoardView::Stroke(int n, double bx, double by, double dx, double dy, double power)
{	
	if(m_bMovingNow) 
		debug("����-���� �����̴µ� �ǵ����..");
		
	m_bMovingNow = true;					// ���� �����̱� �����ߴ�...
	m_nLastPlayer = n;						// ���������� ģ��� .
	m_board.SetCurPlayer(m_nLastPlayer%2);	// ¦����, Ȧ����.
	// ��� �б�
	//	m_board.GetBall(0)->ReadConstant();
	
	PlayResource("IDR_WAVE_STROKE");				// ť��� ���� ĥ�� �Ҹ�

	return m_board.GetBall(n)->Stroke(bx,by,dx,dy,power);
}


double CBoardView::GetDX(int nBallNum)
{
	double x = m_board.GetBall(nBallNum)->GetX();
	double y = m_board.GetBall(nBallNum)->GetY();
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );
	return  (m_dx-x)/d;
}
double CBoardView::GetDY(int nBallNum)
{
	double x = m_board.GetBall(nBallNum)->GetX();
	double y = m_board.GetBall(nBallNum)->GetY();
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );
	return  (m_dy-y)/d;
}



// ���� ����.
void CBoardView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dx = point.x*SCREEN_RATIO;
	m_dy = point.y*SCREEN_RATIO;
	SetCapture();
	m_bLeftCaptured = true;
}
											   
void CBoardView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i;
	CPoint pnt;

	// ���� ���� ���� ã�Ƽ� ���콺 ���µ��� �̵� ��Ų��.
	for(i=0;i<BALL_NUM;i++) {
		pnt = m_board.GetBallPos(i);
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);
		if(abs(pnt.x-point.x)<BALL_VIEW_DIAMETER/2&&abs(pnt.y-point.y)<BALL_VIEW_DIAMETER/2) { // ���콺�� ���� ������
			SetCapture();
			m_bCaptured = true;
			m_nCapturedBallNum = i;
			return;
		}
	}
	// �ƹ����� ������ �ʾ����� �÷��̾� ���� �̵���Ų��.
	m_board.GetBall(m_pParent->m_nBallNum)->SetXY(point.x*SCREEN_RATIO,point.y*SCREEN_RATIO);	
}

void CBoardView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// ���콺 ���´�� ���� �̵� ��Ų��.
	if(m_bCaptured) {
		m_board.GetBall(m_nCapturedBallNum)->SetXY(point.x*SCREEN_RATIO,point.y*SCREEN_RATIO);	
	}
	if(m_bLeftCaptured) {
		m_dx = point.x*SCREEN_RATIO;
		m_dy = point.y*SCREEN_RATIO;
	}
}

void CBoardView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// ���콺�� ���� ���� Ǯ���ش�
	if(m_bCaptured) {
		ReleaseCapture();
		m_bCaptured = false;
	}
}
void CBoardView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// ���� ǥ�� ������ Ǯ���ش�.
	if(m_bLeftCaptured) {
		ReleaseCapture();
		m_bLeftCaptured = false;
	}
}

// ���� ��ġ�� ����ϱ�.
void CBoardView::MemoryBallPos()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		m_pntBallPos[i] = m_board.GetBallPos(i);
	}
}

// ���� ��ġ�� �ǻ츮��. 
void CBoardView::RememberBallPos()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		m_board.GetBall(i)->SetXY(m_pntBallPos[i].x,m_pntBallPos[i].y);
		m_board.GetBall(i)->Stop();
	}
}

// �޸� DC�� ���� ǥ�� ������� �׸���.
void CBoardView::DrawBall2MemDC()
{
	static int			i;
	static CPoint		pnt;
	static CBrush		brush(RGB(0,176,0));
	static bool			bFirst=true;
	static CPoint		oldBallPos[BALL_NUM];
	static CPoint		oldDest;

	if(bFirst) {
//		m_dcMEM.FillRect(CRect(0,0,BOARD_VIEW_WIDTH,BOARD_VIEW_HEIGHT),&brush);
		m_dcMEM.BitBlt(0,0,BOARD_VIEW_WIDTH,BOARD_VIEW_HEIGHT,
			&m_dcMEMBackGnd,0,0,SRCCOPY);
		bFirst = false;
	}

	// �׸��� �����.
	for(i=0;i<BALL_NUM;i++) {
		pnt = oldBallPos[i];
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);

/*
		m_dcMEM.FillRect(
			CRect(
			int(pnt.x-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.y-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.x+BALL_VIEW_DIAMETER/2)+1,
			int(pnt.y+BALL_VIEW_DIAMETER/2)+1),
			&brush);
*/

		m_dcMEM.BitBlt(
			int(pnt.x-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.y-BALL_VIEW_DIAMETER/2)-1,
			(int)BALL_VIEW_DIAMETER+3,
			(int)BALL_VIEW_DIAMETER+3,
			&m_dcMEMBackGnd,
			int(pnt.x-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.y-BALL_VIEW_DIAMETER/2)-1,
			SRCCOPY);
	}

	// ���� ���� ���� �����.
/*
	m_dcMEM.FillRect(
		CRect(CPoint(oldDest-CPoint(10,10)),
			CPoint(oldDest+CPoint(10,10))),&brush);
*/
	m_dcMEM.BitBlt(
		oldDest.x - int(BALL_VIEW_DIAMETER/2)-2,
		oldDest.y - int(BALL_VIEW_DIAMETER/2)-2,
		(int)BALL_VIEW_DIAMETER+3,
		(int)BALL_VIEW_DIAMETER+3,
		&m_dcMEMBackGnd,
		oldDest.x - int(BALL_VIEW_DIAMETER/2)-2,
		oldDest.y - int(BALL_VIEW_DIAMETER/2)-2,
		SRCCOPY);



	// ���� ���༱ �����
	/*
	CPen pen(PS_SOLID,1,RGB(0,176,0));
	CPen *pOldPen = (CPen*)m_dcMEM.SelectObject(&pen);
                                                                                                                                       	pnt = m_board.GetBallPos(0);
	pnt.x = pnt.x/SCREEN_RATIO;
	pnt.y = pnt.y/SCREEN_RATIO;
	m_dcMEM.MoveTo(pnt);
	m_dcMEM.LineTo(dest);
	m_dcMEM.SelectObject(pOldPen);
	*/


	// ���׸��� 
	for(i=0;i<BALL_NUM;i++) {
		double x,y;
		pnt = m_board.GetBallPos(i);
		oldBallPos[i] = pnt;
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);
		// ������ �Ͼ��
		if(i==0) m_pOldBrush = m_dcMEM.SelectObject(m_pWhiteBrush);
		if(i==2) m_pOldBrush = m_dcMEM.SelectObject(m_pRedBrush);

		m_dcMEM.Ellipse(
			int(pnt.x-BALL_VIEW_DIAMETER/2),
			int(pnt.y-BALL_VIEW_DIAMETER/2),
			int(pnt.x+BALL_VIEW_DIAMETER/2),
			int(pnt.y+BALL_VIEW_DIAMETER/2)
			);

		// �귯�� �ǵ�����
		if(i==1||i==3) m_dcMEM.SelectObject(m_pOldBrush);

		// ���� �����.
		if(!m_board.GetBall(i)->GetPoint(x,y)) { // ȭ��Ʒ��� ���� ���� ������.
			x = -x; y = -y;
		}
		m_dcMEM.SetPixel(int(x/SCREEN_RATIO)+pnt.x,int(y/SCREEN_RATIO)+pnt.y,RGB(0,0,0));

		// ������ �ھ��� �׸���.
		if(i==0) m_dcMEM.Ellipse(
			int(pnt.x-BALL_VIEW_DIAMETER/10),
			int(pnt.y-BALL_VIEW_DIAMETER/10),
			int(pnt.x+BALL_VIEW_DIAMETER/10),
			int(pnt.y+BALL_VIEW_DIAMETER/10)
			);


/*		������ ���� ���ְ� �׸��� ��ƾ(����)
		double d = sqrt(x*x+y*y);
		double hangle = acos(d/BALL_RADIUS);
		double angle = atan2(y,x);

		double x2,y2;

		x2 = cos(angle)*cos(hangle)*BALL_RADIUS;
		y2 = sin(angle)*cos(hangle)*BALL_RADIUS;
		m_dcMEM.SetPixel(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y,RGB(0,0,0));
		m_dcMEM.MoveTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);

		if(i==0) {
			x2 = cos(angle)*cos(hangle+0.1)*BALL_RADIUS;
			y2 = sin(angle)*cos(hangle+0.1)*BALL_RADIUS;
			m_dcMEM.MoveTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);

			x2 = cos(angle)*cos(hangle)*BALL_RADIUS+cos(M_PI_2+angle)*5;
			y2 = sin(angle)*cos(hangle)*BALL_RADIUS+sin(M_PI_2+angle)*5;
			m_dcMEM.LineTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);

			x2 = cos(angle)*cos(hangle-0.1)*BALL_RADIUS;
			y2 = sin(angle)*cos(hangle-0.1)*BALL_RADIUS;
			m_dcMEM.LineTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);

			x2 = cos(angle)*cos(hangle)*BALL_RADIUS-cos(M_PI_2+angle)*5;
			y2 = sin(angle)*cos(hangle)*BALL_RADIUS-sin(M_PI_2+angle)*5;
			m_dcMEM.LineTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);

			x2 = cos(angle)*cos(hangle+0.1)*BALL_RADIUS;
			y2 = sin(angle)*cos(hangle+0.1)*BALL_RADIUS;
			m_dcMEM.LineTo(int(x2/SCREEN_RATIO)+pnt.x,int(y2/SCREEN_RATIO)+pnt.y);
		}
*/
	}

	// ���� ���� ���� �׸���.
	CPoint	dest(int(m_dx/SCREEN_RATIO),int(m_dy/SCREEN_RATIO));
	oldDest = dest;
	
	m_pOldBrush = m_dcMEM.SelectObject(m_pGreenBrush);
	m_dcMEM.Ellipse(
		int(dest.x-BALL_VIEW_DIAMETER/2),
		int(dest.y-BALL_VIEW_DIAMETER/2),
		int(dest.x+BALL_VIEW_DIAMETER/2),
		int(dest.y+BALL_VIEW_DIAMETER/2)
		);
	m_dcMEM.SelectObject(m_pOldBrush);
	/*
	m_dcMEM.MoveTo(CPoint(dest-CPoint(6,0)));
	m_dcMEM.LineTo(dest+CPoint(5,0));
	m_dcMEM.MoveTo(CPoint(dest-CPoint(0,6)));
	m_dcMEM.LineTo(dest+CPoint(0,5));
	*/
	m_dcMEM.MoveTo(CPoint(dest-CPoint(6,1)));
	m_dcMEM.LineTo(dest+CPoint(5,-1));
	m_dcMEM.MoveTo(CPoint(dest-CPoint(1,6)));
	m_dcMEM.LineTo(dest+CPoint(-1,5));


	// ���� ���༱ �׸���.
	/*
	pnt = m_board.GetBallPos(0);
	pnt.x = pnt.x/SCREEN_RATIO;
	pnt.y = pnt.y/SCREEN_RATIO;
	m_dcMEM.MoveTo(pnt);
	m_dcMEM.LineTo(dest);
	*/

}

void CBoardView::DrawMemDC2Screen()
{
	static int	i;
	static CPoint pnt;
	static CPoint		oldBallPos[BALL_NUM];
	static CPoint		oldDest;

	CDC* pDC = GetDC();

	// ������ ���� �ִ� ��ġ �����.
	for(i=0;i<BALL_NUM;i++) {
		pnt = oldBallPos[i];
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);
		pDC->BitBlt(
			int(pnt.x-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.y-BALL_VIEW_DIAMETER/2)-1,
			(int)BALL_VIEW_DIAMETER+3,
			(int)BALL_VIEW_DIAMETER+3,
			&m_dcMEM,
			int(pnt.x-BALL_VIEW_DIAMETER/2)-1,
			int(pnt.y-BALL_VIEW_DIAMETER/2)-1,
			SRCCOPY);
	}

	// ���� ���� ���� ���� �����.
	pDC->BitBlt(
		int(oldDest.x-BALL_VIEW_DIAMETER/2),
		int(oldDest.y-BALL_VIEW_DIAMETER/2),
		(int)BALL_VIEW_DIAMETER+1,
		(int)BALL_VIEW_DIAMETER+1,
		&m_dcMEM,
		int(oldDest.x-BALL_VIEW_DIAMETER/2),
		int(oldDest.y-BALL_VIEW_DIAMETER/2),
		SRCCOPY);


	// ���� �ִ� ��ġ�� �޸� DC���� ȭ�鿡 �Ѹ���.
	for(i=0;i<BALL_NUM;i++) {
		pnt = m_board.GetBallPos(i);
		oldBallPos[i] = pnt;
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);

		pDC->BitBlt(
			int(pnt.x-BALL_VIEW_DIAMETER/2),
			int(pnt.y-BALL_VIEW_DIAMETER/2),
			(int)BALL_VIEW_DIAMETER,
			(int)BALL_VIEW_DIAMETER,
			&m_dcMEM,
			int(pnt.x-BALL_VIEW_DIAMETER/2),
			int(pnt.y-BALL_VIEW_DIAMETER/2),
			SRCCOPY);
	}

	// ���� ���� ���� �׸���.
	CPoint	dest(int(m_dx/SCREEN_RATIO),int(m_dy/SCREEN_RATIO));
	oldDest = dest;
	pDC->BitBlt(
		int(dest.x-BALL_VIEW_DIAMETER/2),
		int(dest.y-BALL_VIEW_DIAMETER/2),
		int(BALL_VIEW_DIAMETER)+3,
		int(BALL_VIEW_DIAMETER)+3,
		&m_dcMEM,
		int(dest.x-BALL_VIEW_DIAMETER/2),
		int(dest.y-BALL_VIEW_DIAMETER/2),
		SRCCOPY);


	// �����Ӽ� ó��
	m_nRefreshCount++;
	if(GetTickCount()-m_nStartClock>=1000) {
		m_nRefreshRate = m_nRefreshCount;
		m_nRefreshCount = 0;
		m_nStartClock = GetTickCount();
	}
	char szBuf[100];
	pDC->TextOut(2, 2,szBuf,wsprintf(szBuf,"FPS:%d  ",m_nRefreshRate));
	
	ReleaseDC(pDC);
}

void CBoardView::OnIdle()
{
	Sleep(2);
#ifdef _ON_IDLE_
	DrawBall2MemDC();
	DrawMemDC2Screen();
#endif
}


void CBoardView::DoAI30()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;

		msg ("< AI30 ������ >");
		if(m_ai30.Do()==false) 
		{
			debug("AI30 ERROR!!!");
		}
	
		m_ai30.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
		Stroke(n,bx,by,dx,dy,power);
		FlushMessage();
	}
}


void CBoardView::DoAI50()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;
		int		result;

		msg ("< AI50 ������ >");
		DisableMMTimer();
		if(m_ai50.Do()==false) {
			debug("AI50 ERROR!!!");
		}
		else {
			result = m_ai50.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
			if(result>=10000) {		// ��ǻ�Ͱ� ����� ����� ���Ϸ� �����Ѵ�.
				SaveCurData(n,bx,by,dx,dy,power);
			}
			
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}


void CBoardView::DoAI80()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;
		int		result;

		msg ("< AI80 ������ >");
		DisableMMTimer();
		if(m_ai80.Do()==false) 
		{
			debug("AI80 ERROR!!!");
		}
		else 
		{
				result = m_ai80.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
				Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}

void CBoardView::DoAI100()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;
		int		result;

		msg ("< AI100 ������ >");
		DisableMMTimer();
		if(m_ai100.Do()==false) {
			debug("AI100 ERROR!!!");
		}
		else {
			result = m_ai100.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}
void CBoardView::DoAI150()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;
		int		result;

		msg ("< AI150 ������ >");
		DisableMMTimer();
		if(m_ai150.Do()==false) {
			debug("AI150 ERROR!!!");
		}
		else {
			result = m_ai150.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}

// BPN AI ���� �κ�.

void CBoardView::DoAIBpn()
{
	if(!m_bMovingNow) 
	{
		int n; 
		double bx;
		double by;
		double dx;
		double dy;
		double power;

		msg ("< AI BPN ������ >");
		DisableMMTimer();
		if(m_aiBpn.Do()==false) {
			debug("AIBPN ERROR!!!");
		}
		else {
			m_aiBpn.GetResult(n,bx,by,dx,dy,power);		// ���� ���� �޾ƿ´�.
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}


// ������ ���� ��ġ�� ���� �������� ��ġ�� ���Ϸ� �����Ѵ�.
#define FILE_NAME	"BPN-DATA.txt"
void CBoardView::SaveCurData(int n,double bx, double by, double dx, double dy, double power)
{
	//DisableMMTimer();

	FILE* fp;
	int   nBallNum = n;						// ���.


	fp = fopen(FILE_NAME,"a+");
	if(fp==NULL) {
		AfxMessageBox("���� ���� ����");
		return;
	}
	int i;
	// ����� �Է� �� ���� �Ǵ� �κ�.
	fprintf(fp,"%d\t",nBallNum);
	for(i=0;i<BALL_NUM;i++) {
		fprintf(fp,"%f\t%f\t",
			m_board.GetBall(i)->GetX(),
			m_board.GetBall(i)->GetY()
			);
	}


	// ����� ��¿� ����Ǵ� �κ�.
	fprintf(fp,"%f\t%f\t%f\t%f\t%f\n",
		dx,
		dy,
		bx,
		by,
		power
		);


	fclose(fp);

	//SetMMTimer();
	
}

// ��� �ൿ�ϱ� ������ ���� ��ġ�� ���� ���� ������ �����Ѵ�.
// �̷��� ������ GetCurData()���� ��Ʈ���Ŀ� �����ϰ�, �� �Լ��� Stroke ������ �Ҹ���.
#define HUMAN_FILE_NAME	"HUMAN-BPN-TRAINING-DATA.txt"
void CBoardView::SaveLastData()
{
debug("���ģ ������ ���Ϸ� �����մϴ�. ");
	FILE* fp;

	fp = fopen(HUMAN_FILE_NAME,"a+");
	if(fp==NULL) {
		AfxMessageBox("���� ���� ����");
		return;
	}
	int i;
	// ����� �Է� �� ���� �Ǵ� �κ�.
	fprintf(fp,"%d\t",m_StrokeStatus.nBallNum);
	for(i=0;i<BALL_NUM;i++) {
		fprintf(fp,"%f\t%f\t",
			m_StrokeStatus.BallX[i],
			m_StrokeStatus.BallY[i]
			);
	}

	// ����� ��¿� ����Ǵ� �κ�.
	fprintf(fp,"%f\t%f\t%f\t%f\t%f\n",
		m_StrokeStatus.dx,
		m_StrokeStatus.dy,
		m_StrokeStatus.bx,
		m_StrokeStatus.by,
		m_StrokeStatus.power
		);

	fclose(fp);

}

// CJDRView ���� bx,by �� ��� ���� �ʰ� StrokeStatus�� ��� ���� ȣ���ϴ� �Լ�.
// ��Ÿ �ٸ��������� ������ �ʴ´�.
void CBoardView::GetCurData(int n,double bx,double by,double power)
{

	double dx, dy;

	// ���� ��ġ
	double x = m_board.GetBall(n)->GetX();
	double y = m_board.GetBall(n)->GetY();

	// ���� �������� �Ÿ�
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );

	// dx, �� dy �� ControlDialog���� �Է¹��� �ʰ� ���� ó���Ѵ�.
	// dx �� dy ���  * dx*dx+dy*dy=1 �̴�.!!
	dx = (m_dx-x)/d;
	dy = (m_dy-y)/d;


	int i;
	m_StrokeStatus.nBallNum = n;

	for(i=0;i<BALL_NUM;i++) {
		m_StrokeStatus.BallX[i] = m_board.GetBall(i)->GetX();
		m_StrokeStatus.BallY[i] = m_board.GetBall(i)->GetY();
	}
	m_StrokeStatus.bx = bx;
	m_StrokeStatus.by = by;
	m_StrokeStatus.dx = dx;
	m_StrokeStatus.dy = dy;
	m_StrokeStatus.power = power;

}

void CBoardView::GetCurData(int n,double dx,double dy,double bx,double by,double power)
{
	int i;
	m_StrokeStatus.nBallNum = n;

	for(i=0;i<BALL_NUM;i++) {
		m_StrokeStatus.BallX[i] = m_board.GetBall(i)->GetX();
		m_StrokeStatus.BallY[i] = m_board.GetBall(i)->GetY();
	}
	m_StrokeStatus.bx = bx;
	m_StrokeStatus.by = by;
	m_StrokeStatus.dx = dx;
	m_StrokeStatus.dy = dy;
	m_StrokeStatus.power = power;

}

// �޽����� ���� ���� ������.
void CBoardView::FlushMessage()
{
	MSG msg;
	while(PeekMessage(&msg,m_hWnd,NULL,NULL,PM_REMOVE));
}

void CBoardView::SetMMTimer()
{
/*
	// SET MM TIMER.
    if (timeBeginPeriod(100) == 0)				// TIME_GAP : ���� ó�� �ð�. CBall �� ���ǵǾ� �ִ�.
		m_timerId = timeSetEvent(TIME_GAP,100,CatchMMTimer,
			(DWORD)this->m_hWnd,TIME_PERIODIC);
*/
}

void CBoardView::DisableMMTimer()
{
/*
	timeKillEvent(m_timerId);
	timeEndPeriod(100);
*/
}

void CBoardView::SpeedTest()
{
	DisableMMTimer();
	TimerOn();
	int i;
	for(i=0;i<10000;i++)
		m_board.DoStep(true);	// �� �����̱�.
	Timer("TimerTest");
	SetMMTimer();
}

// Doc ���� ȣ���Ѵ�.
void CBoardView::SetNewGame(int nFirstPlayersScore, int nSecondPlayersScore)
{
	m_nFirstPlayersScore = nFirstPlayersScore;
	m_nSecondPlayersScore = nSecondPlayersScore;
	msg("������ ����.");
	msg("����: %d��, �İ�: %d��.",
		m_nFirstPlayersScore,
		m_nSecondPlayersScore);
}

