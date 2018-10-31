// AI150.cpp: implementation of the CAI150 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AI150.h"
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

CAI150::CAI150()
{
	m_pBoard = NULL;
	m_nBallNum = 0;
	m_nErrorFactor = 8;		// 숫자가 클수록 정확도가 높다.
}

CAI150::~CAI150()
{
}

bool CAI150::Do()
{

	ASSERT(m_pParent);
	m_nBallNum = m_pParent->m_pParent->m_nBallNum==0?1:0;

	debug("err fc:%d",m_nErrorFactor);
	m_arrSD.RemoveAll();

	AddAimRedBallCourse();	// 빨간공을 먼저 때리는 코스를 집어 넣어 본다.

	int i;
	for(i=0;i<20;i++) {
		AddFirstCourse();
		ExpandNode();
//		SelectTop20();
		SelectTop10();
		if(m_arrSD[0].nEvalValue>=EVAL_GOOD+10) { // 어느 정도 길을 확보했으면 멈춘다..
			debug("적당한 길을 찾았다!! loop:%d",i);
			break;	
		}
	}
	if(m_arrSD.GetSize()==0) return false;

	CheckGayasiCourse();

	return true;
}

#define NUM_OF_CHECK_GAYASI 30

void CAI150::CheckGayasiCourse()
{
	if(m_arrSD.GetSize()==0) return;
	// 제일 좋은 결과만 일단 남겨둔다. ( 이미 SelectTop20에 의해 제일 좋은 결과만 남겨져 있어야 한다.)
	m_arrSD.SetSize(1);

debug("가야시전 값:%d",m_arrSD[0].nEvalValue);


	int nEvalResult;
	int j,k;
//	int nBlockToWall;

	CBoard	board;
	STROKE_DATA	 sd;

	for(j=0;j<NUM_OF_CHECK_GAYASI;j++) {
		board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

		sd = m_arrSD[0];						// 제일 좋은 결과..
		sd.power =  rand()%MAX_BALL_V0_SPEED*0.8;
//		sd.bx = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
//		sd.by = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
//		sd.dx = cos(theta*3.14/180);											//방향은 건드리지 않는다.
//		sd.dy = sin(theta*3.14/180);

		board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

//		nBlockToWall = 0;
		// 공이 멈출때까지 충분이 계산.
		for(k=0;k<5000;k++) {
			board.DoStep(true);	// 공 움직이기.
			if(board.IsStop()) {
				break;
			}
		}
		sd.nEvalValue = nEvalResult = board.GetGayasiEvalResult();

		m_arrSD.Add(sd);
debug("가야시 계산후:%d",nEvalResult);
	}
	debug("가야시 계산후 알다마 후보 갯수 :%d",m_arrSD.GetSize());

}

void CAI150::SelectTop10()
{
debug("상위 10 고르기.");
	int i,j;
	int loop = m_arrSD.GetSize(); 

	if(loop<=10) return;

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
	// 상위 10 짜르기.
	m_arrSD.SetSize(10);
debug("현재 TOP:%d",m_arrSD[0].nEvalValue);

}
