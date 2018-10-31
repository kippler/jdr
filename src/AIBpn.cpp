// AIBpn.cpp: implementation of the CAIBpn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "AIBpn.h"
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
#define		INPUT_UNIT_NUM			8
#define		OUTPUT_UNIT_NUM			5

#define		HIDDEN_LAYER_NUM		4
#define		HIDDEN_LAYER_UNIT_NUM	70


CAIBpn::CAIBpn()
{
	m_szFileName = "HUMAN-BPN-DATA.DAT";		// BPN-내용 저장 파일.
	m_pBpn = new CBpn(INPUT_UNIT_NUM,HIDDEN_LAYER_NUM,HIDDEN_LAYER_UNIT_NUM,OUTPUT_UNIT_NUM);
debug("BPN 데이타를 파읽에서 읽기..");
	m_pBpn->LoadFromFile(m_szFileName);

	m_arrIn.SetSize(MAX_INPUT_NODE);
	m_arrOut.SetSize(MAX_OUT_NODE);
	m_arrTarget.SetSize(MAX_OUT_NODE);

}

CAIBpn::~CAIBpn()
{
	delete m_pBpn;
}

bool CAIBpn::Do()
{
	CBoard	board;
	board = *m_pBoard;						// m_ball에 관한 내용만 복사한다.

	int i;
debug("BPN 시작-좌표입력.");
	// 입력 요소 - 공의 좌표.
	for(i=0;i<BALL_NUM;i++) {
		m_arrIn[i*2  ] = board.GetBall(i)->GetX();
		m_arrIn[i*2+1] = board.GetBall(i)->GetX();
	}
	
debug("BPN normalization");
	Normalization();

	m_pBpn->TestNet(m_arrIn,m_arrOut,m_arrTarget);

	return true;
}

void CAIBpn::Normalization()
{
	m_arrIn[0]/=_BOARD_WIDTH;
	m_arrIn[1]/=_BOARD_HEIGHT;
	m_arrIn[2]/=_BOARD_WIDTH;
	m_arrIn[3]/=_BOARD_HEIGHT;
	m_arrIn[4]/=_BOARD_WIDTH;
	m_arrIn[5]/=_BOARD_HEIGHT;
	m_arrIn[6]/=_BOARD_WIDTH;
	m_arrIn[7]/=_BOARD_HEIGHT;
}

int  CAIBpn::GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power)
{
debug("BPN GETRESULT");
	n =  0;
	dx = m_arrOut[0]*2-1;
	dy = m_arrOut[1]*2-1;
	bx = m_arrOut[2]*BALL_DIAMETER-BALL_RADIUS;
	by = m_arrOut[3]*BALL_DIAMETER-BALL_RADIUS;
	power = m_arrOut[4]*MAX_BALL_V0_SPEED;
	return 0;
}