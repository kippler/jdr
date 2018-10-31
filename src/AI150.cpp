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
	m_nErrorFactor = 8;		// ���ڰ� Ŭ���� ��Ȯ���� ����.
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

	AddAimRedBallCourse();	// �������� ���� ������ �ڽ��� ���� �־� ����.

	int i;
	for(i=0;i<20;i++) {
		AddFirstCourse();
		ExpandNode();
//		SelectTop20();
		SelectTop10();
		if(m_arrSD[0].nEvalValue>=EVAL_GOOD+10) { // ��� ���� ���� Ȯ�������� �����..
			debug("������ ���� ã�Ҵ�!! loop:%d",i);
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
	// ���� ���� ����� �ϴ� ���ܵд�. ( �̹� SelectTop20�� ���� ���� ���� ����� ������ �־�� �Ѵ�.)
	m_arrSD.SetSize(1);

debug("���߽��� ��:%d",m_arrSD[0].nEvalValue);


	int nEvalResult;
	int j,k;
//	int nBlockToWall;

	CBoard	board;
	STROKE_DATA	 sd;

	for(j=0;j<NUM_OF_CHECK_GAYASI;j++) {
		board = *m_pBoard;						// m_ball�� ���� ���븸 �����Ѵ�.
		board.m_nCurBallNum = m_nBallNum;
		board.ClearBallEvent();

		sd = m_arrSD[0];						// ���� ���� ���..
		sd.power =  rand()%MAX_BALL_V0_SPEED*0.8;
//		sd.bx = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
//		sd.by = rand()%int(BALL_DIAMETER)-BALL_DIAMETER/2;
//		sd.dx = cos(theta*3.14/180);											//������ �ǵ帮�� �ʴ´�.
//		sd.dy = sin(theta*3.14/180);

		board.GetBall(m_nBallNum)->Stroke(sd.bx,sd.by,sd.dx,sd.dy,sd.power);

//		nBlockToWall = 0;
		// ���� ���⶧���� ����� ���.
		for(k=0;k<5000;k++) {
			board.DoStep(true);	// �� �����̱�.
			if(board.IsStop()) {
				break;
			}
		}
		sd.nEvalValue = nEvalResult = board.GetGayasiEvalResult();

		m_arrSD.Add(sd);
debug("���߽� �����:%d",nEvalResult);
	}
	debug("���߽� ����� �˴ٸ� �ĺ� ���� :%d",m_arrSD.GetSize());

}

void CAI150::SelectTop10()
{
debug("���� 10 ����.");
	int i,j;
	int loop = m_arrSD.GetSize(); 

	if(loop<=10) return;

	STROKE_DATA sdTemp;

	// ���� ��Ʈ.
	for(i=0;i<loop-1;i++) {
		for(j=i+1;j<loop;j++) {
			if(m_arrSD[i].nEvalValue<m_arrSD[j].nEvalValue) {
				sdTemp = m_arrSD[i];
				m_arrSD[i] = m_arrSD[j];
				m_arrSD[j] = sdTemp;
			}
		}
	}
	// ���� 10 ¥����.
	m_arrSD.SetSize(10);
debug("���� TOP:%d",m_arrSD[0].nEvalValue);

}
