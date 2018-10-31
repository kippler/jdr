// AI80.cpp: implementation of the CAI80 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AI80.h"
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

CAI80::CAI80()
{
	m_pBoard = NULL;
	m_nBallNum = 0;
}

CAI80::~CAI80()
{
}

bool CAI80::Do()
{
	m_arrSD.RemoveAll();
	AddFirstAldamaCourse();
	AddPowerControl();
	AddRotaionControl();
	if(m_arrSD.GetSize()==0) return false;
	return true;
}

#define RED_BALL_1	2
#define RED_BALL_2  3

void CAI80::AddFirstAldamaCourse()
{
	int nEvalResult;
	int i,j;

	CBoard	board;
	STROKE_DATA	 sd;
	double theta;
	const int		nDefaultPower = 1000;
	int		nEvalCutLine = 500;

	double initTheta;
	double d1, d2;

	board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
	board.m_nCurBallNum = m_nBallNum;

	d1 = GetDist(m_pBoard->GetBall(m_nBallNum)->GetX(),	// 현재공과 첫번째 빨간공과의 거리.
		m_pBoard->GetBall(m_nBallNum)->GetY(),
		m_pBoard->GetBall(RED_BALL_1)->GetX(),
		m_pBoard->GetBall(RED_BALL_1)->GetY());

	d2 = GetDist(m_pBoard->GetBall(m_nBallNum)->GetX(),	// 현재공과 두번째 빨간공과의 거리.
		m_pBoard->GetBall(m_nBallNum)->GetY(),
		m_pBoard->GetBall(RED_BALL_2)->GetX(),
		m_pBoard->GetBall(RED_BALL_2)->GetY());


	double objBallX, objBallY;	// 가까운 빨간공의 좌표
	double curBallX, curBallY;	// 현재 자신의 공

	curBallX = m_pBoard->GetBall(m_nBallNum)->GetX();
	curBallY = m_pBoard->GetBall(m_nBallNum)->GetY();

	// 가까운공 찾기.
	if(d1<d2) {
		objBallX = m_pBoard->GetBall(RED_BALL_1)->GetX();
		objBallY = m_pBoard->GetBall(RED_BALL_1)->GetY();
	}
	else {
		objBallX = m_pBoard->GetBall(RED_BALL_2)->GetX();
		objBallY = m_pBoard->GetBall(RED_BALL_2)->GetY();
	}
	// 현재공에서 가까운 공에 대한 각도.
	initTheta = atan2(objBallY-curBallY,objBallX-curBallX);


	for(i=0;i<40;i++) {

		board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

//		theta = initTheta+(((double(rand()%360))/10/3-6)/180*M_PI);	// 6도 (좌우 3도씩 회전 검사.)
		theta = initTheta+(((double(rand()%1000))/1000*30-15)/180*M_PI);	// 6도 (좌우 3도씩 회전 검사.)
debug("i:%f theta:%f",initTheta,theta);
		sd.dx = cos(theta);
		sd.dy = sin(theta);
		
		sd.nBallNum = m_nBallNum;
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
debug("Add  :%d",nEvalResult);
		}
	}

	debug("총 초기 알다마 후보 갯수 :%d",m_arrSD.GetSize());


	for(i=m_arrSD.GetSize()-1;i>0;i--) {
		if(m_arrSD[i].nEvalValue<nEvalCutLine*0.8) 
			m_arrSD.RemoveAt(i);
	}

	debug("총 초기 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

int  CAI80::GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power)
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


double CAI80::GetDist(double x1, double y1, double x2, double y2)
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}
void CAI80::AddPowerControl()
{
	int nEvalResult;
	int i,j,k;

	CBoard	board;
	STROKE_DATA	 sd;
	int loop = m_arrSD.GetSize(); 

	for(i=0;i<loop;i++) {
		for(j=0;j<5;j++) {
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
				m_arrSD[i].nEvalValue++;	// 루트도 증가.
debug("Add:%d",nEvalResult);
			}
		}
	}
	debug("파워 조정후 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

void CAI80::AddRotaionControl()
{
	int nEvalResult;
	int i,j,k;

	CBoard	board;
	STROKE_DATA	 sd;
	int loop = m_arrSD.GetSize(); 

	for(i=0;i<loop;i++) {
		for(j=0;j<5;j++) {
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
				m_arrSD[i].nEvalValue++;	// 루트도 증가.
debug("Add:%d",nEvalResult);
			}
		}
	}
	debug("BX,BY 조정후 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

