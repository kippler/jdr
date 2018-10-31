#if !defined(AFX_BOARDVIEW_H__0EBE9836_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
#define AFX_BOARDVIEW_H__0EBE9836_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Board.h"
#include "ControlDlg.h"
#include "AI30.h"
#include "AI50.h"
#include "AI80.h"
#include "AI100.h"
#include "AI150.h"
#include "AIBpn.h"
#include "PlaySnd.h"			// 효과음을 위해 추가 

#ifdef _TEST_	    
	#define SCREEN_RATIO		2
#else
	#define SCREEN_RATIO		4
#endif


class CJDRView;

// PARENT WINDOW 위치에 생성될 좌표.
#define		BOARD_VIEW_LEFT		31
#define		BOARD_VIEW_TOP		155


#define BOARD_WIDTH			(CBoard::m_width)//2540     /3 = 840	/4-> 637.5
#define BOARD_HEIGHT		(CBoard::m_height)//1270    /3 = 423 	/4-> 317.5
#define BOARD_VIEW_WIDTH	int(BOARD_WIDTH/SCREEN_RATIO)
#define BOARD_VIEW_HEIGHT	int(BOARD_HEIGHT/SCREEN_RATIO)


// 현재 공의 위치와 공을 때리는 방법을 저장하는 구조체
struct StrokeStatus {
	int nBallNum;
	double BallX[BALL_NUM];
	double BallY[BALL_NUM];
	double dx,dy;
	double bx,by;
	double power;
};


class CBoardView : public CWnd
{
protected :
	CBitmap		m_bmBack;		// m_dcMEM에 사용될 BITMAP
	CBitmap		m_bmBackGnd;	// 다이 그림을 가지고 있을 BITMAP
	CDC			m_dcMEM;		// 화면에 뿌릴 그림을 저장할 DC
	CDC			m_dcMEMBackGnd;	// 다이 그림을 저장할 DC
    CBitmap     *m_pOldBitmap;
	CBitmap		*m_pOldBitmapBackGnd;

	CBrush		*m_pRedBrush;
	CBrush		*m_pWhiteBrush;
	CBrush		*m_pGreenBrush;
	CBrush		*m_pOldBrush;

	int			m_timerId;		// MM timer id

	double		m_dx,m_dy;		// 공의 진행 위치(destination).

	bool		m_bCaptured;	// 공을 마우스로 잡고 있나.
	bool		m_bLeftCaptured;	// 표적지 끌고 다니나..
	int			m_nCapturedBallNum;	// 마우스로 들고 다니는 공 번호.

	CPoint		m_pntBallPos[BALL_NUM];	// 공의 위치 기억할 변수

	int			m_nRefreshRate;		// Frame 수를 체크하기 위한 변수들.
	int			m_nRefreshCount;
	int			m_nStartClock;

	CAI30		m_ai30;				// AI ( 30짜리 )
	CAI50		m_ai50;				// AI ( 50짜리 )
	CAI80		m_ai80;				// AI ( 80짜리 )
	CAI100		m_ai100;			// AI ( 100짜리 )
	CAI150		m_ai150;			// AI ( 150짜리 )	// 100+가야시
	CAIBpn		m_aiBpn;			// AI ( BPN )


	int			m_nLastPlayer;		// 마지막으로 공을 친 사람.!!!! ( 짝수면 선공, 홀수면 후공. ) 0,1,2...순으로 나간다.

	int			m_nFirstPlayersScore;	// 선공의 스코어.
	int			m_nSecondPlayersScore;	// 후공의 스코어.

public :
	bool		m_bMovingNow;		// 공이 현재 움직이고 있다. !!! ( 따라서 건들지마..)
	CJDRView	*m_pParent;

// Construction
public:
	CBoardView();
	StrokeStatus m_StrokeStatus;

// Attributes
public:
	CBoard		m_board;			// 당구 다이.

// Operations
public:
	double		GetDX(int nBallNum);
	double		GetDY(int nBallNum);
	void		SetDX(double dx) { m_dx = dx; }		// 수정요망.
	void		SetDY(double dy) { m_dy = dy; }
//	CControlDlg	*m_pDlg;			

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoardView)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SpeedTest();
	void DoAI80();
	void DoAI50();
	void DoAI30();
	void DoAIBpn();
	void OnIdle();
	void DrawMemDC2Screen();
	void RememberBallPos();
	void MemoryBallPos();

	// n:공번호 bx,by:공때리는 위치 dx,dy:방향 power:강도
	bool Stroke(int n,double bx, double by, double power);
	bool Stroke(int n,double bx, double by, double dx, double dy, double power);
	void Test2();
	void Test();
	bool CreateWin(CWnd * pParent, int top , int left );
	virtual ~CBoardView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBoardView)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public :
	void DoAI150();
	void SetNewGame(int nFirstPlayersScore, int nSecondPlayersScore);
	void DoAI100();
	void SetMMTimer();
	void DisableMMTimer();
	void GetCurData(int n,double bx,double by,double power);
private:
	void GetCurData(int n,double dx,double dy,double bx,double by,double power);
	void SaveLastData();
	void SaveCurData(int n,double bx, double by, double dx, double dy, double power);
	void FlushMessage();
	void DrawBall2MemDC();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOARDVIEW_H__0EBE9836_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
