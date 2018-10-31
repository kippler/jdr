// AI100.cpp: implementation of the CAI100 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AI100.h"
#include "Board.h"
#include "BoardView.h"
#include "JDRDoc.h"
#include "JDRView.h"
#include "Ball.h"
#include "Util.h"
#include <math.H>
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI100::CAI100()
{
	m_pBoard = NULL;
	m_pParent = NULL;
	m_nBallNum = 0;
	m_nErrorFactor = 3;
}

CAI100::~CAI100()
{
}

bool CAI100::Do()
{

	ASSERT(m_pParent);
	m_nBallNum = m_pParent->m_pParent->m_nBallNum==0?1:0;
	
	msg("AI100 생각중......");
	m_arrSD.RemoveAll();

	int i;
	AddAimRedBallCourse();	// 빨간공을 먼저 때리는 코스를 집어 넣어 본다.
	for(i=0;i<7;i++) {
		AddFirstCourse();
		ExpandNode();
		SelectTop20();
	}
	if(m_arrSD.GetSize()==0) return false;
	return true;
}

#define RED_BALL_1	2
#define RED_BALL_2  3

#define		ERROR_RANGE_OF_POWER	0.1		// 10%
#define		ERROR_RANGE_OF_BXY		0.1		// 10%
#define		ERROR_RANGE_OF_DXY		0.05		// 5%

int  CAI100::GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power)
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

	// 원래의 데이타.
	n =  m_arrSD[nBestPos].nBallNum;
	bx = m_arrSD[nBestPos].bx;
	by = m_arrSD[nBestPos].by;
	dx = m_arrSD[nBestPos].dx;
	dy = m_arrSD[nBestPos].dy;
	power = m_arrSD[nBestPos].power;

	// 일부러 에러를 추가한다.
	bx    = bx + (double(rand())/RAND_MAX-0.5) * BALL_DIAMETER * ERROR_RANGE_OF_BXY/m_nErrorFactor;
	by    = by + (double(rand())/RAND_MAX-0.5) * BALL_DIAMETER * ERROR_RANGE_OF_BXY/m_nErrorFactor;
	dx    = dx + (double(rand())/RAND_MAX-0.5) * ERROR_RANGE_OF_DXY/m_nErrorFactor;
	dy    = dy + (double(rand())/RAND_MAX-0.5) * ERROR_RANGE_OF_DXY/m_nErrorFactor;
	power = power + (double(rand())/RAND_MAX-0.5) * MAX_BALL_V0_SPEED * ERROR_RANGE_OF_POWER/m_nErrorFactor;


debug("BestResult:%d",nBestResult);
	return nBestResult;
}


double CAI100::GetDist(double x1, double y1, double x2, double y2)
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}


#define		MAX_REDBALL_AIM_COURSE	40
// 가까운 빨간공을 치는 코스를 선택.	--- ai80에서 뜯어옴.
void CAI100::AddAimRedBallCourse()
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


	for(i=0;i<MAX_REDBALL_AIM_COURSE;i++) {

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
//		if(nEvalResult>nEvalCutLine*0.8) {
			m_arrSD.Add(sd);
//			nEvalCutLine = max(nEvalCutLine,nEvalResult);
debug("Add  :%d",nEvalResult);
//		}
	}

	debug("총 초기 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}



#define	FIRST_INITIAL_NODE	10
// 랜덤하게 초기 코스를 집어 넣는다.
void CAI100::AddFirstCourse()
{
	int i,j;
	int nBlockToWall;

	CBoard	board;
	STROKE_DATA	 sd;
	double theta;

	for(i=0;i<FIRST_INITIAL_NODE;i++) {
		board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

		// 랜덤하게 초기 공의 방향 설정.
		theta = rand()%360;
		sd.nBallNum = m_nBallNum;
		sd.bx = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
		sd.by = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
		sd.dx = cos(theta*3.14/180);
		sd.dy = sin(theta*3.14/180);
		sd.power = rand()%MAX_BALL_V0_SPEED*0.7;

		board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

		nBlockToWall = 0;
		for(j=0;j<300;j++) {
			board.DoStep(true);	// 빠르게(fast==true) 공 움직이기.
			if(board.IsStop()) break;
			if(board.m_bBlockToWall) break;	// 벽에 박으면 무조건 스탑.
/*
			if(board.m_bBlockToWall) {	  // 초기 공이 벽에 2번까지 부딪히는것을 허용.
				nBlockToWall++;
				if(nBlockToWall>1) break;
			}
*/
		}
		sd.nEvalValue = board.GetEvalResult();
debug("AI평가결과:%d",sd.nEvalValue);
		m_arrSD.Add(sd);
	}

	debug("총 초기 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

#define		NUM_OF_EXPAND	5
// 리스트에 있는 값들을 약간씩 변화시켜 노드를 확장한다.
void CAI100::ExpandNode()
{
	int nEvalResult;
	int i,j,k;
	int nBlockToWall;

	CBoard	board;
	STROKE_DATA	 sd;
	int loop = m_arrSD.GetSize(); 

	for(i=0;i<loop;i++) {
		for(j=0;j<NUM_OF_EXPAND;j++) {
			board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
			board.m_nCurBallNum = m_nBallNum;
			board.ClearBallEvent();

			sd = m_arrSD[i];
//			if(sd.nEvalValue<EVAL_NOT_BAD) break;	// 싹수가 노란 노드는 확장하지도 않는다.

			sd.power = sd.power + (double(rand())/RAND_MAX-0.5) * MAX_BALL_V0_SPEED * ERROR_RANGE_OF_POWER;
			sd.bx    = sd.bx + (double(rand())/RAND_MAX-0.5) * BALL_DIAMETER * ERROR_RANGE_OF_BXY;
			sd.by    = sd.by + (double(rand())/RAND_MAX-0.5) * BALL_DIAMETER * ERROR_RANGE_OF_BXY;
			sd.dx    = sd.dx + (double(rand())/RAND_MAX-0.5) * ERROR_RANGE_OF_DXY;
			sd.dy    = sd.dy + (double(rand())/RAND_MAX-0.5) * ERROR_RANGE_OF_DXY;
//			sd.power = rand()%(MAX_BALL_V0_SPEED/5*3);

			board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

			nBlockToWall = 0;
			for(k=0;k<300;k++) {
				board.DoStep(true);	// 공 움직이기.
				if(board.IsStop()) break;
				if(board.m_bBlockToWall) {
					nBlockToWall++;
					if(nBlockToWall>2) break;
				}
			}
			sd.nEvalValue = nEvalResult = board.GetEvalResult();

			// 만일 자식 노드의 결과가 괜찮으면 루트 노드에 평가값 증가.
			m_arrSD.Add(sd);
			if(nEvalResult>=EVAL_GOOD) {	// EVAL_GOOD : board.h에 정의
				m_arrSD[i].nEvalValue++;
debug("부모노드증가:%d",m_arrSD[i].nEvalValue);
			}
			else {
debug("부:%d, 자:%d",m_arrSD[i].nEvalValue,nEvalResult);
				if(nEvalResult==0) {	// 만일 확장된 노드가 죽는 길이면 부모 노드는 위험성이 큰 노드이다.
					m_arrSD[i].nEvalValue--;
				}
			}
debug("Add:%d",nEvalResult);
		}
	}
	debug("파워 조정후 알다마 후보 갯수 :%d",m_arrSD.GetSize());
}

// 상위 20개의 길을 고른다.
void CAI100::SelectTop20()
{
debug("상위 20 고르기.");
	int i,j;
	int loop = m_arrSD.GetSize(); 

	if(loop<=20) return;

	STROKE_DATA sdTemp;

	// 버블 소트.
	for(i=0;i<loop-1;i++) {
		for(j=i+1;j<loop;j++) {
			if(m_arrSD[i].nEvalValue<m_arrSD[j].nEvalValue) {
				sdTemp = m_arrSD[i];
				m_arrSD[i] = m_arrSD[j];
				m_arrSD[j] = sdTemp;
			}
		}
	}
	// 상위 20 짜르기.
	m_arrSD.SetSize(20);
debug("현재 TOP:%d",m_arrSD[0].nEvalValue);
}

void CAI100::SetParent(CBoardView * pParent)
{
	m_pParent = pParent;
}
