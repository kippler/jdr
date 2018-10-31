// Board.cpp: implementation of the CBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "Board.h"
#include "Util.h"
#include "PlaySnd.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define RED1_BALL_NUM 2
#define RED2_BALL_NUM 3
#define EVAL_MAX_DIST   (CBoard::m_width+CBoard::m_height)
#define EVAL_MAX_DIST_SQUARE  (EVAL_MAX_DIST*EVAL_MAX_DIST)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoard::CBoard()
{

	for(int i=0;i<BALL_NUM;i++)
		m_ball[i].SetParent(this);
	InitBallPos();
	m_nCurBallNum = 0;
	m_nBallHitCount = 0;

	InitB2BDistance();
	m_bBlockToWall = false;
}

CBoard::~CBoard()
{
}

double CBoard::GetWidth()
{
	return m_width;
}

double CBoard::GetHeight()
{
	return m_height;
}

CPoint CBoard::GetBallPos(int n)
{
	if(n<0||n>=BALL_NUM) return NULL;
	return CPoint((int)m_ball[n].GetX(),(int)m_ball[n].GetY());
}


CBall* CBoard::GetBall(int n)
{
	return &m_ball[n];
}

bool CBoard::DoStep(bool bFast)
{
	int i,j;
	double d;
	BoardEvent	be;

	// 각공들의 이동.
	for(i=0;i<BALL_NUM;i++) {
		m_ball[i].DoMove(bFast);
	//	if(!bFast)
	//		if(m_ball[i].m_bBlockToWall);
	//			PlayResource("WAVE_TOCK");		// 공이 벽에 충돌시 효과음.
	}

	m_bBlockToWall = m_ball[m_nCurBallNum].m_bBlockToWall;	// 벽에 충돌여부 반영
	
	// 공들사이의 최단거리 찾기.
	static double  daB2BDistanceSquare[BALL_NUM][BALL_NUM];	// double array of Ball 2 Ball distance, 공과 공 사이의 최근접 거리.
	for(i=0;i<BALL_NUM-1;i++) {
		for(j=i+1;j<BALL_NUM;j++) {
			daB2BDistanceSquare[i][j] = GetBallDistanceSquare(i,j);
/*
if(i==0&&j==2) {
	if(m_daB2BDistanceSquare[i][j]>daB2BDistanceSquare[i][j])
		debug("1번빨간공 거리 재세팅:%d %d",
			int(m_daB2BDistanceSquare[i][j]),
			int(daB2BDistanceSquare[i][j]));
}
*/
			m_daB2BDistanceSquare[i][j] = min(m_daB2BDistanceSquare[i][j],daB2BDistanceSquare[i][j]);
		}
	}

#define BALL_DIAMETER_SQUARE (BALL_DIAMETER*BALL_DIAMETER)

	// 공과 공의 충돌 적용.
	for(i=0;i<BALL_NUM-1;i++) {
		for(j=i+1;j<BALL_NUM;j++) {
			// 공과 공사이의 거리 계산.
//			d = GetBallDistanceSquare(i,j);
			d = daB2BDistanceSquare[i][j];
			// 만약 충돌이면...
			if(d<BALL_DIAMETER_SQUARE) {
				// i에는 j를 넘기고 j 에는 임시 공을 만들어서 넘긴다.
				// ( 변수가 바뀌는걸 방지 하기 위해서 )
				CBall ball= m_ball[i];
				m_ball[i].Collision(m_ball[j]);
				m_ball[j].Collision(ball);
				be.nEventNum = BE_COLLISION;
				be.nBallNum1 = i;
				be.nBallNum2 = j;
				m_arrBE.Add(be);
				// 공과공의 최소 거리는 0이 된다.
				m_daB2BDistanceSquare[i][j] = 0;

				// 내공과 빨간공이 부딛혔을 경우. 다른 빨간공과의 거리는(나중에 결과 함수를 리턴할때) 의미가 없어지므로 MAX값으로 재 세팅.
				// 이때 이미 다른 빨간공과도 부딛혔으면 그냥 나둔다.
				if(i==m_nCurBallNum) {
					if(j==RED1_BALL_NUM) {	// 내공과 빨간공1과의 충돌
						if(m_daB2BDistanceSquare[m_nCurBallNum][RED2_BALL_NUM]!=0) {
							m_daB2BDistanceSquare[m_nCurBallNum][RED2_BALL_NUM] = EVAL_MAX_DIST_SQUARE;
//debug("2번공과의 거리 재세팅");
						}
					}
					else if(j==RED2_BALL_NUM){	// 내공과 빨간공 2와의 충돌.
						if(m_daB2BDistanceSquare[m_nCurBallNum][RED1_BALL_NUM]!=0) {
							m_daB2BDistanceSquare[m_nCurBallNum][RED1_BALL_NUM] = EVAL_MAX_DIST_SQUARE;
//debug("1번공과의 거리 재세팅");
						}
					}
				}

				// 틱 소리를 낸다.
				if (!bFast)	
				{
					m_nBallHitCount++;
					PlayBallSound();
				}
//					PlayResource("IDR_WAVE_BALL1");
				//sndPlaySound
//debug("충돌:%d-%d",i,j);
			}
		}
	}
	return true;
}

// 공들이 전부 멈췄나?
bool CBoard::IsStop()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		if(m_ball[i].IsStop()==false) return false;
	}

	m_nBallHitCount = 0;

	return true;
}

// 공을 친후의 결과를 리턴한다.
BoardResult CBoard::GetResult()
{
	BoardEvent	beCR1 = {BE_COLLISION,m_nCurBallNum,RED1_BALL_NUM};				// 내공과 빨간공
	BoardEvent	beCR2 = {BE_COLLISION,m_nCurBallNum,RED2_BALL_NUM};				// 내공과 빨간공2
	BoardEvent	beRR  = {BE_COLLISION,RED1_BALL_NUM,RED2_BALL_NUM};				// 빨간공끼리
	BoardEvent	beCO1 = {BE_COLLISION,m_nCurBallNum,(m_nCurBallNum==0)?1:0};	// 내공과 상대공1
	BoardEvent	beCO2 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,m_nCurBallNum};	// 내공과 상대공2
	BoardEvent	beOR1 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,RED1_BALL_NUM};	// 상대공과 빨간공1
	BoardEvent	beOR2 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,RED2_BALL_NUM};	// 상대공과 빨간공2

	if(IsCollision(beCO1)||IsCollision(beCO2)) {				// 상대공을 쳤을경우
//debug("실점!!!");
		return BR_LOST_SCORE;
	}
	if(IsCollision(beCR1)&&IsCollision(beCR2)) {				// 두개의 빨간공을 다 쳤을 경우
//debug("득점!!!");
		return BR_SCORE;
	}
	if(IsCollision(beCR1)||IsCollision(beCR2)) {				// 빨간공중 하나라도 쳤을경우.
//debug("밥벌이!!!");
		return BR_NONE;
	}

	// 쓰리 쿳션은 나중에..
//debug("실점!!!");												// 아무것도 못 쳤을 경우.
	return BR_LOST_SCORE_NONE;
	
}

void CBoard::ClearBallEvent()
{
	m_arrBE.RemoveAll();			// 모든 공의 충돌 이벤트와
	InitB2BDistance();				// 공의 거리계산을 초기화 한다.
}



#define EVAL_MAX_DIST   (CBoard::m_width+CBoard::m_height)
#define EVAL_DIST_POINT 0.1

// 평가 함수 결과 리턴
int CBoard::GetEvalResult()
{

	int i,j;
	for(i=0;i<BALL_NUM-1;i++) {
		for(j=i+1;j<BALL_NUM;j++) {
//debug("A:%d B:%d - [%2.2f]",i,j,m_daB2BDistanceSquare[i][j]);
		}
	}

	int res=GetResult();

	double dRed1Dist = sqrt(m_daB2BDistanceSquare[m_nCurBallNum][RED1_BALL_NUM]);
	double dRed2Dist = sqrt(m_daB2BDistanceSquare[m_nCurBallNum][RED2_BALL_NUM]);

	if(res==BR_SCORE) return EVAL_GOOD;			// + 가야시() 가 들어가야 함..
	if(res==BR_LOST_SCORE) return EVAL_BAD;		// + 겐세이() 가 들어가햐 함.
	if(res==BR_NONE) {
/*
debug("1:%d 2:%d r1:%d r2:%d",
	int((EVAL_MAX_DIST-dRed1Dist)*EVAL_DIST_POINT),
	int((EVAL_MAX_DIST-dRed2Dist)*EVAL_DIST_POINT),
	int(dRed1Dist),
	int(dRed2Dist)
	);
*/
		return EVAL_NOT_BAD + int(
			(EVAL_MAX_DIST-dRed1Dist)*EVAL_DIST_POINT +
			(EVAL_MAX_DIST-dRed2Dist)*EVAL_DIST_POINT);
	}
	if(res==BR_LOST_SCORE_NONE) {
		return EVAL_BAD + int(
			(EVAL_MAX_DIST-dRed1Dist)*EVAL_DIST_POINT +
			(EVAL_MAX_DIST-dRed2Dist)*EVAL_DIST_POINT);
	}
	return 0;	
}

int CBoard::GetGayasiEvalResult()
{
	int dist;		// 빨간공 사이의 거리.
	int val;		// 가야시 값.
	// 멈춘후의 공과 공사이의 거리를 가지고 계산을 한다.
	dist = (int)GetBallDistance(RED1_BALL_NUM,RED2_BALL_NUM);
	val =  (EVAL_MAX_DIST-dist)/100;	

	return GetEvalResult()+val;
}


// BoardEvent 스트럭처의 비교 함수.
bool operator==(const BoardEvent be1, const BoardEvent be2) 
{
	if(be1.nEventNum==be2.nEventNum &&
		be1.nBallNum1==be2.nBallNum1 &&
		be1.nBallNum2==be2.nBallNum2) return true;
	return false;
}

bool CBoard::IsCollision(const BoardEvent & be)
{
	int i;
	for(i=0;i<m_arrBE.GetSize();i++) {
		if(m_arrBE.GetAt(i)==be) return true;
	}
	return false;
}

void CBoard::InitB2BDistance()
{
	int i,j;
	// 공들사이의 거리를 임의의 최대값으로 지정한다.
	for(i=0;i<BALL_NUM;i++) {
		for(j=0;j<BALL_NUM;j++) {
			m_daB2BDistanceSquare[i][j] = EVAL_MAX_DIST_SQUARE;
		}
	}
}

double CBoard::GetBallDistance(const int a, const int b)
{
	double i,j;
	i = (m_ball[a].GetX()-m_ball[b].GetX());
	j = (m_ball[a].GetY()-m_ball[b].GetY());
	return sqrt(i*i+j*j);
//	return ((m_ball[a].GetX()-m_ball[b].GetX())*(m_ball[a].GetX()-m_ball[b].GetX())
//		+ (m_ball[a].GetY()-m_ball[b].GetY())*(m_ball[a].GetY()-m_ball[b].GetY()));
//	return sqrt((m_ball[a].GetX()-m_ball[b].GetX())*(m_ball[a].GetX()-m_ball[b].GetX())
//		+ (m_ball[a].GetY()-m_ball[b].GetY())*(m_ball[a].GetY()-m_ball[b].GetY()));
}
double CBoard::GetBallDistanceSquare(const int a, const int b)
{
	double i,j;
	i = (m_ball[a].GetX()-m_ball[b].GetX());
	j = (m_ball[a].GetY()-m_ball[b].GetY());
	return i*i+j*j;
//	return ((m_ball[a].GetX()-m_ball[b].GetX())*(m_ball[a].GetX()-m_ball[b].GetX())
//		+ (m_ball[a].GetY()-m_ball[b].GetY())*(m_ball[a].GetY()-m_ball[b].GetY()));
//	return sqrt((m_ball[a].GetX()-m_ball[b].GetX())*(m_ball[a].GetX()-m_ball[b].GetX())
//		+ (m_ball[a].GetY()-m_ball[b].GetY())*(m_ball[a].GetY()-m_ball[b].GetY()));
}

const CBoard& CBoard::operator=(const CBoard &a) 
{
	int i;
	if(this==&a) return *this;
	for(i=0;i<BALL_NUM;i++) {
		m_ball[i] = a.m_ball[i];
	}
	m_nCurBallNum = a.m_nCurBallNum;
	return *this;
}


void CBoard::SetCurPlayer(int nPlayer)
{
	m_nCurBallNum = nPlayer;
}

// 공의 위치를 초기화 한다.
void CBoard::InitBallPos()
{
	m_ball[0].SetXY(_BOARD_WIDTH*2/8,_BOARD_HEIGHT/2+BALL_DIAMETER+2);
	m_ball[1].SetXY(_BOARD_WIDTH*7/8,_BOARD_HEIGHT/2);
	m_ball[2].SetXY(_BOARD_WIDTH*2/8,_BOARD_HEIGHT/2);
	m_ball[3].SetXY(_BOARD_WIDTH*6/8,_BOARD_HEIGHT/2);
}

// 부딪칠때마다 소리의 강도를 약화 시킨다. 
void CBoard::PlayBallSound()
{
	switch (m_nBallHitCount)
	{
		case 1 : PlayResource("IDR_WAVE_BALL1");
				 break;
		case 2 : PlayResource("IDR_WAVE_BALL2");
				 break;
		case 3 : PlayResource("IDR_WAVE_BALL3");
                 break;
		default : PlayResource("IDR_WAVE_BALL4");
	}
}