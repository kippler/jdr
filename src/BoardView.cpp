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

#define _ON_IDLE_			// 화면의 갱신 종류를 결정한다.
							// define 되어 있으면 CWinApp의 OnIdle로 화면을 갱신하고
							// 아니면 MMTimer로 갱신한다.

#define BALL_DIAMETER		65.5
#define BALL_VIEW_DIAMETER	(BALL_DIAMETER/SCREEN_RATIO)


#define TIMER_ID_SCREEN_REFRESH		1		// 화면 갱신용.
#define TIMER_ID_DO_PROCESS			2		// 내부 처리용.
#define TIMER_ID_BALL_CHECK			3		// 공의 상태 체크용
#define SCREEN_FREFRESH_GAP			20		// 30ms 마다 화면 갱신.


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


/* MMTimer 를 받아서 좀더 빠른 WM_TIMER를 윈도우에 보낸다. */
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

	m_ai30.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_ai30.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

	m_ai50.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_ai50.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

	m_ai80.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_ai80.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

	m_ai100.SetParent(this);
	m_ai100.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_ai100.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

	m_ai150.SetParent(this);
	m_ai150.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_ai150.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

	m_aiBpn.SetBoard(&m_board); // 다이의 포인터를 넘긴다.
	m_aiBpn.SetBallNum(m_board.m_nCurBallNum==0?1:0);	// 현재 게이머의 반대공을 지정한다.

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

/* Parent Window 를 받아서 Window를 생성 한다.*/
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

	// CONTROL DIALOG 생성
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
	m_bmBackGnd.LoadBitmap(IDB_BOARD);     // bitmap 읽어 들이기.

	// 메모리 DC 생성.
	m_dcMEM.CreateCompatibleDC(pDC);
	m_dcMEMBackGnd.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);
	m_pOldBitmap = m_dcMEM.SelectObject(&m_bmBack);
	m_pOldBitmapBackGnd = m_dcMEMBackGnd.SelectObject(&m_bmBackGnd);

	// 메모리 DC에 최초로 그리기
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
// 단순히 메모리 DC를 화면에 뿌려 버린다.
void CBoardView::OnPaint() 
{
	CPaintDC dc(this); 

	// DRAW TO SCREEN 
	dc.BitBlt(0,0,BOARD_VIEW_WIDTH,BOARD_VIEW_HEIGHT,
		&m_dcMEM,0,0,SRCCOPY);
}

// 공 뿌리기. (랜덤하게)
void CBoardView::Test()
{
	m_board.m_nBallHitCount = 0;
	for(int i=0;i<BALL_NUM;i++) 
		m_board.GetBall(i)->SetVXY(rand()%MAX_BALL_V0_SPEED*2-MAX_BALL_V0_SPEED,
			rand()%MAX_BALL_V0_SPEED*2-MAX_BALL_V0_SPEED);
}
// Timer 를 받아서 공도 움직이고 화면도 갱신한다.
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
//debug("현재플레이어[%d]에 대한평가결과:%d",m_nLastPlayer,eval);
					{
						CString	str;
						int		&nCurScore = (m_nLastPlayer%2==0) ? m_nFirstPlayersScore  : m_nSecondPlayersScore;
						str.Format("현재 당신[%d]는 ",m_nLastPlayer);
						if(eval>=EVAL_GOOD) 
						{
							str+= "득점을 올렸습니다.";
							nCurScore-=10;
						}
						else if(eval>=EVAL_NOT_BAD) 
						{
							str+= "득점하지 못했습니다.";
						}
						else 
						{
							str+= "실점을 하셨습니다.";
							nCurScore+=10;
						}
						msg(str);
						str.Format("[ 점수 : %d ]",nCurScore);
						msg(str);
						msg("---------------------------------------------------------------");
					}
					m_board.ClearBallEvent();
//DoAI50();									// BPN용 데이타 저장시.
//if(eval>=10000) SaveLastData();				// 평가 결과가 괜잖으면 파일로 저장.
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
						if(count++==5) {	// 화면 갱신
							count = 0;
							DrawBall2MemDC();
							DrawMemDC2Screen();
						}
				#endif
				#endif
						m_board.DoStep();	// 공 움직이기.
						break;
	}
}

// 콘트롤 다이알로그 보여주기.
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


// ControlDlg에서 호출한다. ==> CJDRView에서 호출로 변경됨.
bool CBoardView::Stroke(int n, double bx, double by, double power)
{
if(m_bMovingNow) 
	debug("에러-공이 움직이는데 건들었어..");


	double dx, dy;
	// 상수 읽기
//	m_board.GetBall(0)->ReadConstant();

	// 공의 위치
	double x = m_board.GetBall(n)->GetX();
	double y = m_board.GetBall(n)->GetY();

	// 공과 목적지의 거리
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );

	// dx, 와 dy 는 ControlDialog에서 입력받지 않고 직접 처리한다.
	// dx 와 dy 계산  * dx*dx+dy*dy=1 이다.!!
	dx = (m_dx-x)/d;
	dy = (m_dy-y)/d;

	GetCurData(n,dx,dy,bx,by,power);

	m_bMovingNow = true;			// 공이 움직이기 시작했다...
	m_nLastPlayer = n;				// 마지막으로 친사람 .
	m_board.SetCurPlayer(m_nLastPlayer%2);		// 짝수팀, 홀수팀.

	PlayResource("IDR_WAVE_STROKE");				// 큐대로 공을 칠때 소리

	return m_board.GetBall(n)->Stroke(bx,by,dx,dy,power);
}


// AI에서 사용한다.
bool CBoardView::Stroke(int n, double bx, double by, double dx, double dy, double power)
{	
	if(m_bMovingNow) 
		debug("에러-공이 움직이는데 건들었어..");
		
	m_bMovingNow = true;					// 공이 움직이기 시작했다...
	m_nLastPlayer = n;						// 마지막으로 친사람 .
	m_board.SetCurPlayer(m_nLastPlayer%2);	// 짝수팀, 홀수팀.
	// 상수 읽기
	//	m_board.GetBall(0)->ReadConstant();
	
	PlayResource("IDR_WAVE_STROKE");				// 큐대로 공을 칠때 소리

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



// 방향 지정.
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

	// 현재 눌린 공을 찾아서 마우스 가는데로 이동 시킨다.
	for(i=0;i<BALL_NUM;i++) {
		pnt = m_board.GetBallPos(i);
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);
		if(abs(pnt.x-point.x)<BALL_VIEW_DIAMETER/2&&abs(pnt.y-point.y)<BALL_VIEW_DIAMETER/2) { // 마우스로 공을 잡으면
			SetCapture();
			m_bCaptured = true;
			m_nCapturedBallNum = i;
			return;
		}
	}
	// 아무공도 눌리지 않았으면 플레이어 공을 이동시킨다.
	m_board.GetBall(m_pParent->m_nBallNum)->SetXY(point.x*SCREEN_RATIO,point.y*SCREEN_RATIO);	
}

void CBoardView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// 마우스 가는대로 공을 이동 시킨다.
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
	// 마우스를 떼면 공을 풀어준다
	if(m_bCaptured) {
		ReleaseCapture();
		m_bCaptured = false;
	}
}
void CBoardView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// 공의 표적 방향을 풀어준다.
	if(m_bLeftCaptured) {
		ReleaseCapture();
		m_bLeftCaptured = false;
	}
}

// 공의 위치를 기억하기.
void CBoardView::MemoryBallPos()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		m_pntBallPos[i] = m_board.GetBallPos(i);
	}
}

// 공의 위치를 되살리기. 
void CBoardView::RememberBallPos()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		m_board.GetBall(i)->SetXY(m_pntBallPos[i].x,m_pntBallPos[i].y);
		m_board.GetBall(i)->Stop();
	}
}

// 메모리 DC에 공과 표적 방향등을 그린다.
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

	// 그린공 지우기.
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

	// 공의 진행 방향 지우기.
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



	// 공의 진행선 지우기
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


	// 공그리기 
	for(i=0;i<BALL_NUM;i++) {
		double x,y;
		pnt = m_board.GetBallPos(i);
		oldBallPos[i] = pnt;
		pnt.x = int(pnt.x/SCREEN_RATIO);
		pnt.y = int(pnt.y/SCREEN_RATIO);
		// 빨간공 하얀공
		if(i==0) m_pOldBrush = m_dcMEM.SelectObject(m_pWhiteBrush);
		if(i==2) m_pOldBrush = m_dcMEM.SelectObject(m_pRedBrush);

		m_dcMEM.Ellipse(
			int(pnt.x-BALL_VIEW_DIAMETER/2),
			int(pnt.y-BALL_VIEW_DIAMETER/2),
			int(pnt.x+BALL_VIEW_DIAMETER/2),
			int(pnt.y+BALL_VIEW_DIAMETER/2)
			);

		// 브러쉬 되돌리기
		if(i==1||i==3) m_dcMEM.SelectObject(m_pOldBrush);

		// 공에 점찍기.
		if(!m_board.GetBall(i)->GetPoint(x,y)) { // 화면아래에 공의 점이 있을때.
			x = -x; y = -y;
		}
		m_dcMEM.SetPixel(int(x/SCREEN_RATIO)+pnt.x,int(y/SCREEN_RATIO)+pnt.y,RGB(0,0,0));

		// 쩜공을 멋없게 그리기.
		if(i==0) m_dcMEM.Ellipse(
			int(pnt.x-BALL_VIEW_DIAMETER/10),
			int(pnt.y-BALL_VIEW_DIAMETER/10),
			int(pnt.x+BALL_VIEW_DIAMETER/10),
			int(pnt.y+BALL_VIEW_DIAMETER/10)
			);


/*		쩜공의 쩜을 멋있게 그리는 루틴(느림)
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

	// 공의 진행 방향 그리기.
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


	// 공의 진행선 그리기.
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

	// 이전에 공이 있던 위치 지우기.
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

	// 이전 공의 진행 방향 지우기.
	pDC->BitBlt(
		int(oldDest.x-BALL_VIEW_DIAMETER/2),
		int(oldDest.y-BALL_VIEW_DIAMETER/2),
		(int)BALL_VIEW_DIAMETER+1,
		(int)BALL_VIEW_DIAMETER+1,
		&m_dcMEM,
		int(oldDest.x-BALL_VIEW_DIAMETER/2),
		int(oldDest.y-BALL_VIEW_DIAMETER/2),
		SRCCOPY);


	// 공이 있는 위치만 메모리 DC에서 화면에 뿌리기.
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

	// 공의 진행 방향 그리기.
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


	// 프레임수 처리
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

		msg ("< AI30 생각중 >");
		if(m_ai30.Do()==false) 
		{
			debug("AI30 ERROR!!!");
		}
	
		m_ai30.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
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

		msg ("< AI50 생각중 >");
		DisableMMTimer();
		if(m_ai50.Do()==false) {
			debug("AI50 ERROR!!!");
		}
		else {
			result = m_ai50.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
			if(result>=10000) {		// 컴퓨터가 계산한 결과를 파일로 저장한다.
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

		msg ("< AI80 생각중 >");
		DisableMMTimer();
		if(m_ai80.Do()==false) 
		{
			debug("AI80 ERROR!!!");
		}
		else 
		{
				result = m_ai80.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
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

		msg ("< AI100 생각중 >");
		DisableMMTimer();
		if(m_ai100.Do()==false) {
			debug("AI100 ERROR!!!");
		}
		else {
			result = m_ai100.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
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

		msg ("< AI150 생각중 >");
		DisableMMTimer();
		if(m_ai150.Do()==false) {
			debug("AI150 ERROR!!!");
		}
		else {
			result = m_ai150.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}

// BPN AI 수행 부분.

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

		msg ("< AI BPN 생각중 >");
		DisableMMTimer();
		if(m_aiBpn.Do()==false) {
			debug("AIBPN ERROR!!!");
		}
		else {
			m_aiBpn.GetResult(n,bx,by,dx,dy,power);		// 전부 값을 받아온다.
			Stroke(n,bx,by,dx,dy,power);
		}

		SetMMTimer();
		FlushMessage();
	}
}


// 현재의 공의 배치와 공을 때리려는 위치를 파일로 저장한다.
#define FILE_NAME	"BPN-DATA.txt"
void CBoardView::SaveCurData(int n,double bx, double by, double dx, double dy, double power)
{
	//DisableMMTimer();

	FILE* fp;
	int   nBallNum = n;						// 사람.


	fp = fopen(FILE_NAME,"a+");
	if(fp==NULL) {
		AfxMessageBox("파일 저장 실패");
		return;
	}
	int i;
	// 여기는 입력 에 관계 되는 부분.
	fprintf(fp,"%d\t",nBallNum);
	for(i=0;i<BALL_NUM;i++) {
		fprintf(fp,"%f\t%f\t",
			m_board.GetBall(i)->GetX(),
			m_board.GetBall(i)->GetY()
			);
	}


	// 여기는 출력에 관계되는 부분.
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

// 방금 행동하기 직전의 공의 배치와 공을 때린 정보를 저장한다.
// 이러한 정보는 GetCurData()에서 스트럭쳐에 저장하고, 이 함수는 Stroke 직전에 불린다.
#define HUMAN_FILE_NAME	"HUMAN-BPN-TRAINING-DATA.txt"
void CBoardView::SaveLastData()
{
debug("방금친 정보를 파일로 저장합니다. ");
	FILE* fp;

	fp = fopen(HUMAN_FILE_NAME,"a+");
	if(fp==NULL) {
		AfxMessageBox("파일 저장 실패");
		return;
	}
	int i;
	// 여기는 입력 에 관계 되는 부분.
	fprintf(fp,"%d\t",m_StrokeStatus.nBallNum);
	for(i=0;i<BALL_NUM;i++) {
		fprintf(fp,"%f\t%f\t",
			m_StrokeStatus.BallX[i],
			m_StrokeStatus.BallY[i]
			);
	}

	// 여기는 출력에 관계되는 부분.
	fprintf(fp,"%f\t%f\t%f\t%f\t%f\n",
		m_StrokeStatus.dx,
		m_StrokeStatus.dy,
		m_StrokeStatus.bx,
		m_StrokeStatus.by,
		m_StrokeStatus.power
		);

	fclose(fp);

}

// CJDRView 에서 bx,by 를 계산 하지 않고 StrokeStatus를 얻기 위해 호출하는 함수.
// 기타 다른곳에서는 쓰이지 않는다.
void CBoardView::GetCurData(int n,double bx,double by,double power)
{

	double dx, dy;

	// 공의 위치
	double x = m_board.GetBall(n)->GetX();
	double y = m_board.GetBall(n)->GetY();

	// 공과 목적지의 거리
	double d = sqrt( (x-m_dx)*(x-m_dx) + (y-m_dy)*(y-m_dy) );

	// dx, 와 dy 는 ControlDialog에서 입력받지 않고 직접 처리한다.
	// dx 와 dy 계산  * dx*dx+dy*dy=1 이다.!!
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

// 메시지를 전부 지워 버린다.
void CBoardView::FlushMessage()
{
	MSG msg;
	while(PeekMessage(&msg,m_hWnd,NULL,NULL,PM_REMOVE));
}

void CBoardView::SetMMTimer()
{
/*
	// SET MM TIMER.
    if (timeBeginPeriod(100) == 0)				// TIME_GAP : 내부 처리 시간. CBall 에 정의되어 있다.
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
		m_board.DoStep(true);	// 공 움직이기.
	Timer("TimerTest");
	SetMMTimer();
}

// Doc 에서 호출한다.
void CBoardView::SetNewGame(int nFirstPlayersScore, int nSecondPlayersScore)
{
	m_nFirstPlayersScore = nFirstPlayersScore;
	m_nSecondPlayersScore = nSecondPlayersScore;
	msg("새게임 시작.");
	msg("선공: %d점, 후공: %d점.",
		m_nFirstPlayersScore,
		m_nSecondPlayersScore);
}

