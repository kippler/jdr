// AI50.cpp: implementation of the CAI50 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AI50.h"
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

CAI50::CAI50()
{
	m_pBoard = NULL;
	m_nBallNum = 0;
}

CAI50::~CAI50()
{
}

bool CAI50::Do()
{
	m_arrSD.RemoveAll();
	AddFirstAldamaCourse();
	AddPowerControl();
	AddRotaionControl();
	if(m_arrSD.GetSize()==0) return false;
	return true;
}

void CAI50::AddFirstAldamaCourse()
{
	int nEvalResult;
	int j;

	CBoard	board;
	STROKE_DATA	 sd;
	int theta;
	const int		nDefaultPower = 1000;
	int		nEvalCutLine = 500;
	for(theta=0;theta<360;theta++) {
		board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

		sd.nBallNum = m_nBallNum;
		sd.dx = cos(theta*3.14/180);
		sd.dy = sin(theta*3.14/180);
		sd.bx = 0;
		sd.by = 0;
		sd.power = nDefaultPower;

		board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

		for(j=0;j<300;j++) {
			board.DoStep(true);	// 빠르게(fast==true) 공 움직이기.
			if(board.IsStop()) break;
			if(board.m_bBlockToWall) break;
		}
		sd.nEvalValue = nEvalResult = board.GetEvalResult();
		if(nEvalResult>nEvalCutLine*0.8) {
			m_arrSD.Add(sd);
			nEvalCutLine = max(nEvalCutLine,nEvalResult);
debug("Add:%d",nEvalResult);
			;
		}
	}

	int i;
	for(i=m_arrSD.GetSize()-1;i>0;i--) {
		if(m_arrSD[i].nEvalValue<nEvalCutLine*0.8) 
			m_arrSD.RemoveAt(i);
	}

	debug("총 초기 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

int  CAI50::GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power)
{
	int i;
	int nBestResult = 0;
	int nBestPos = 0;
	for(i=0;i<m_arrSD.GetSize();i++) {
		if(m_arrSD[i].nEvalValue>=nBestResult) {
			nBestResult = m_arrSD[i].nEvalValue;
			nBestPos = i;
		}
	}
	n =  m_arrSD[nBestPos].nBallNum;
	bx = m_arrSD[nBestPos].bx;
	by = m_arrSD[nBestPos].by;
	dx = m_arrSD[nBestPos].dx;
	dy = m_arrSD[nBestPos].dy;
	power = m_arrSD[nBestPos].power;

	return nBestResult;

debug("BestResult:%d",nBestResult);
}

void CAI50::AddPowerControl()
{
	int nEvalResult;
	int i,j,k;

	CBoard	board;
	STROKE_DATA	 sd;
	int loop = m_arrSD.GetSize(); 

	for(i=0;i<loop;i++) {
		for(j=0;j<10;j++) {
			board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
			board.m_nCurBallNum = m_nBallNum;
			board.ClearBallEvent();

			sd = m_arrSD[i];
			sd.power = rand()%(MAX_BALL_V0_SPEED/5*3);

			board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

			for(k=0;k<300;k++) {
				board.DoStep(true);	// 공 움직이기.
				if(board.IsStop()) break;
//				if(board.m_bBlockToWall) break;
			}
			sd.nEvalValue = nEvalResult = board.GetEvalResult();
			if(nEvalResult>m_arrSD[i].nEvalValue) {
				m_arrSD.Add(sd);
debug("Add:%d",nEvalResult);
			}
		}
	}
	debug("파워 조정후 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

void CAI50::AddRotaionControl()
{
	int nEvalResult;
	int i,j,k;

	CBoard	board;
	STROKE_DATA	 sd;
	int loop = m_arrSD.GetSize(); 

	for(i=0;i<loop;i++) {
		for(j=0;j<10;j++) {
			board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
			board.m_nCurBallNum = m_nBallNum;
			board.ClearBallEvent();

			sd = m_arrSD[i];
			sd.bx = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
			sd.by = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;

			board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

			for(k=0;k<300;k++) {
				board.DoStep(true);	// 공 움직이기.
				if(board.IsStop()) break;
//				if(board.m_bBlockToWall) break;
			}
			sd.nEvalValue = nEvalResult = board.GetEvalResult();
			if(nEvalResult>m_arrSD[i].nEvalValue) {
				m_arrSD.Add(sd);
debug("Add:%d",nEvalResult);
			}
		}
	}
	debug("BX,BY 조정후 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}
