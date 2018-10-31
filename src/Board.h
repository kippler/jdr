// Board.h: interface for the CBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOARD_H__BAF3FEA1_11AE_11D2_B8EB_E814512230CE__INCLUDED_)
#define AFX_BOARD_H__BAF3FEA1_11AE_11D2_B8EB_E814512230CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Ball.h"
#include <afxtempl.h>

#define BALL_NUM 4


// DEFINE BALL EVENT NUMBER
#define BE_COLLISION	1
#define	BE_CLOSE		2

struct BoardEvent {
	int	nEventNum;
	int nBallNum1;
	int nBallNum2;
};




// 공을 친후의 결과 
enum BoardResult { 
	BR_NONE,				// 밥값만하다.
	BR_SCORE,				// 알다마 얻다.
	BR_THREE_CUSION,		// 쓰리 쿠션 성공.
	BR_LOST_SCORE,			// 실점.(상대공 쳤을경우)
	BR_LOST_SCORE_NONE};	// 실점.(아무공도 치지 못하였을 경우)


#define EVAL_GOOD		10000
#define EVAL_NOT_BAD	1000
#define EVAL_BAD		0


#define		_BOARD_WIDTH		2550
#define		_BOARD_HEIGHT	1270

class CBoard  
{
private :
	CBall	m_ball[BALL_NUM];
	CArray<BoardEvent,BoardEvent>	m_arrBE;	// ARRAY OF BALL EVENT
	double  m_daB2BDistanceSquare[BALL_NUM][BALL_NUM];	// double array of Ball 2 Ball distance, 공과 공 사이의 최근접 거리.

public :
	int		m_nCurBallNum;						// 현재 공격자의 공(0혹은1)
	bool	m_bBlockToWall;						// 공격자의 공이 벽에 충돌했나?
private : // FUNCTIONS
	void InitB2BDistance();
	bool IsCollision(const BoardEvent& be);

public:
	int m_nBallHitCount;
	void PlayBallSound(void);
	void InitBallPos();
	int GetGayasiEvalResult();
	void SetCurPlayer(int nPlayer);
	void ClearBallEvent();
	int GetEvalResult();
	double GetBallDistance(const int a, const int b);
	double GetBallDistanceSquare(const int a, const int b);
	BoardResult GetResult();
	bool IsStop();

	const CBoard& operator=(const CBoard &a);

#ifdef _TEST_
	enum {m_width = 1000,m_height=500};		// 상수.	테스트용.
#else
	enum {m_width = _BOARD_WIDTH,m_height=_BOARD_HEIGHT};		// 상수.
#endif

	bool DoStep(bool bFast = false);
	CBall* GetBall(int n);
	CPoint GetBallPos(int n);
	double GetHeight();
	double GetWidth();
	CBoard();
	virtual ~CBoard();

};

#endif // !defined(AFX_BOARD_H__BAF3FEA1_11AE_11D2_B8EB_E814512230CE__INCLUDED_)
