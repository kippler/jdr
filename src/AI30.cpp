// AI30.cpp: implementation of the CAI30 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AI30.h"
#include "Board.h"
#include "Ball.h"
#include "Util.h"
#include <math.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI30::CAI30()
{
	m_pBoard = NULL;
	m_nBallNum = 0;
}

CAI30::~CAI30()
{

}

void CAI30::SetBoard(CBoard * pBoard)
{
	m_pBoard = pBoard;
}

void CAI30::SetBallNum(int nBallNum)
{
	m_nBallNum = nBallNum;
	m_strokeData.nBallNum = m_nBallNum;
}

bool CAI30::Do()
{
	int nBestResult=-1;
	int nEvalResult;
	int i,j;
	CBoard	board;

	double bx;
	double by;
	double dx;
	double dy;
	double power;
	double theta;


	for(i=0;i<30;i++) {
		board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

		theta = rand()%360;
		bx = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
		by = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
		dx = cos(theta*3.14/180);
		dy = sin(theta*3.14/180);
		power = rand()%MAX_BALL_V0_SPEED;

		board.GetBall(m_nBallNum)->Stroke(bx,by,dx,dy,power);

		for(j=0;j<10000;j++) {
			board.DoStep();	// 공 움직이기.
			if(board.IsStop()) break;
		}
if(j==10000) debug("ERROR AT LOOP!!!!");

		nEvalResult = board.GetEvalResult();
debug("AI평가결과:%d",nEvalResult);
		if(nEvalResult>nBestResult) {
			m_strokeData.bx = bx;
			m_strokeData.by = by;
			m_strokeData.dx = dx;
			m_strokeData.dy = dy;
			m_strokeData.power = power;
			nBestResult = nEvalResult;
		}
	}
debug("AI최고 평가 결과:%d",nBestResult);
	return true;
}

int CAI30::GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power)
{
	n = m_strokeData.nBallNum;
	bx = m_strokeData.bx;
	by = m_strokeData.by;
	dx = m_strokeData.dx;
	dy = m_strokeData.dy;
	power = m_strokeData.power;
	return 0;
}
