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




// ���� ģ���� ��� 
enum BoardResult { 
	BR_NONE,				// �䰪���ϴ�.
	BR_SCORE,				// �˴ٸ� ���.
	BR_THREE_CUSION,		// ���� ��� ����.
	BR_LOST_SCORE,			// ����.(���� �������)
	BR_LOST_SCORE_NONE};	// ����.(�ƹ����� ġ�� ���Ͽ��� ���)


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
	double  m_daB2BDistanceSquare[BALL_NUM][BALL_NUM];	// double array of Ball 2 Ball distance, ���� �� ������ �ֱ��� �Ÿ�.

public :
	int		m_nCurBallNum;						// ���� �������� ��(0Ȥ��1)
	bool	m_bBlockToWall;						// �������� ���� ���� �浹�߳�?
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
	enum {m_width = 1000,m_height=500};		// ���.	�׽�Ʈ��.
#else
	enum {m_width = _BOARD_WIDTH,m_height=_BOARD_HEIGHT};		// ���.
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
