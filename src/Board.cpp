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

	// �������� �̵�.
	for(i=0;i<BALL_NUM;i++) {
		m_ball[i].DoMove(bFast);
	//	if(!bFast)
	//		if(m_ball[i].m_bBlockToWall);
	//			PlayResource("WAVE_TOCK");		// ���� ���� �浹�� ȿ����.
	}

	m_bBlockToWall = m_ball[m_nCurBallNum].m_bBlockToWall;	// ���� �浹���� �ݿ�
	
	// ��������� �ִܰŸ� ã��.
	static double  daB2BDistanceSquare[BALL_NUM][BALL_NUM];	// double array of Ball 2 Ball distance, ���� �� ������ �ֱ��� �Ÿ�.
	for(i=0;i<BALL_NUM-1;i++) {
		for(j=i+1;j<BALL_NUM;j++) {
			daB2BDistanceSquare[i][j] = GetBallDistanceSquare(i,j);
/*
if(i==0&&j==2) {
	if(m_daB2BDistanceSquare[i][j]>daB2BDistanceSquare[i][j])
		debug("1�������� �Ÿ� �缼��:%d %d",
			int(m_daB2BDistanceSquare[i][j]),
			int(daB2BDistanceSquare[i][j]));
}
*/
			m_daB2BDistanceSquare[i][j] = min(m_daB2BDistanceSquare[i][j],daB2BDistanceSquare[i][j]);
		}
	}

#define BALL_DIAMETER_SQUARE (BALL_DIAMETER*BALL_DIAMETER)

	// ���� ���� �浹 ����.
	for(i=0;i<BALL_NUM-1;i++) {
		for(j=i+1;j<BALL_NUM;j++) {
			// ���� �������� �Ÿ� ���.
//			d = GetBallDistanceSquare(i,j);
			d = daB2BDistanceSquare[i][j];
			// ���� �浹�̸�...
			if(d<BALL_DIAMETER_SQUARE) {
				// i���� j�� �ѱ�� j ���� �ӽ� ���� ���� �ѱ��.
				// ( ������ �ٲ�°� ���� �ϱ� ���ؼ� )
				CBall ball= m_ball[i];
				m_ball[i].Collision(m_ball[j]);
				m_ball[j].Collision(ball);
				be.nEventNum = BE_COLLISION;
				be.nBallNum1 = i;
				be.nBallNum2 = j;
				m_arrBE.Add(be);
				// �������� �ּ� �Ÿ��� 0�� �ȴ�.
				m_daB2BDistanceSquare[i][j] = 0;

				// ������ �������� �ε����� ���. �ٸ� ���������� �Ÿ���(���߿� ��� �Լ��� �����Ҷ�) �ǹ̰� �������Ƿ� MAX������ �� ����.
				// �̶� �̹� �ٸ� ���������� �ε������� �׳� ���д�.
				if(i==m_nCurBallNum) {
					if(j==RED1_BALL_NUM) {	// ������ ������1���� �浹
						if(m_daB2BDistanceSquare[m_nCurBallNum][RED2_BALL_NUM]!=0) {
							m_daB2BDistanceSquare[m_nCurBallNum][RED2_BALL_NUM] = EVAL_MAX_DIST_SQUARE;
//debug("2�������� �Ÿ� �缼��");
						}
					}
					else if(j==RED2_BALL_NUM){	// ������ ������ 2���� �浹.
						if(m_daB2BDistanceSquare[m_nCurBallNum][RED1_BALL_NUM]!=0) {
							m_daB2BDistanceSquare[m_nCurBallNum][RED1_BALL_NUM] = EVAL_MAX_DIST_SQUARE;
//debug("1�������� �Ÿ� �缼��");
						}
					}
				}

				// ƽ �Ҹ��� ����.
				if (!bFast)	
				{
					m_nBallHitCount++;
					PlayBallSound();
				}
//					PlayResource("IDR_WAVE_BALL1");
				//sndPlaySound
//debug("�浹:%d-%d",i,j);
			}
		}
	}
	return true;
}

// ������ ���� ���質?
bool CBoard::IsStop()
{
	int i;
	for(i=0;i<BALL_NUM;i++) {
		if(m_ball[i].IsStop()==false) return false;
	}

	m_nBallHitCount = 0;

	return true;
}

// ���� ģ���� ����� �����Ѵ�.
BoardResult CBoard::GetResult()
{
	BoardEvent	beCR1 = {BE_COLLISION,m_nCurBallNum,RED1_BALL_NUM};				// ������ ������
	BoardEvent	beCR2 = {BE_COLLISION,m_nCurBallNum,RED2_BALL_NUM};				// ������ ������2
	BoardEvent	beRR  = {BE_COLLISION,RED1_BALL_NUM,RED2_BALL_NUM};				// ����������
	BoardEvent	beCO1 = {BE_COLLISION,m_nCurBallNum,(m_nCurBallNum==0)?1:0};	// ������ ����1
	BoardEvent	beCO2 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,m_nCurBallNum};	// ������ ����2
	BoardEvent	beOR1 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,RED1_BALL_NUM};	// ������ ������1
	BoardEvent	beOR2 = {BE_COLLISION,(m_nCurBallNum==0)?1:0,RED2_BALL_NUM};	// ������ ������2

	if(IsCollision(beCO1)||IsCollision(beCO2)) {				// ������ �������
//debug("����!!!");
		return BR_LOST_SCORE;
	}
	if(IsCollision(beCR1)&&IsCollision(beCR2)) {				// �ΰ��� �������� �� ���� ���
//debug("����!!!");
		return BR_SCORE;
	}
	if(IsCollision(beCR1)||IsCollision(beCR2)) {				// �������� �ϳ��� �������.
//debug("�����!!!");
		return BR_NONE;
	}

	// ���� ����� ���߿�..
//debug("����!!!");												// �ƹ��͵� �� ���� ���.
	return BR_LOST_SCORE_NONE;
	
}

void CBoard::ClearBallEvent()
{
	m_arrBE.RemoveAll();			// ��� ���� �浹 �̺�Ʈ��
	InitB2BDistance();				// ���� �Ÿ������ �ʱ�ȭ �Ѵ�.
}



#define EVAL_MAX_DIST   (CBoard::m_width+CBoard::m_height)
#define EVAL_DIST_POINT 0.1

// �� �Լ� ��� ����
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

	if(res==BR_SCORE) return EVAL_GOOD;			// + ���߽�() �� ���� ��..
	if(res==BR_LOST_SCORE) return EVAL_BAD;		// + �ռ���() �� ���� ��.
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
	int dist;		// ������ ������ �Ÿ�.
	int val;		// ���߽� ��.
	// �������� ���� �������� �Ÿ��� ������ ����� �Ѵ�.
	dist = (int)GetBallDistance(RED1_BALL_NUM,RED2_BALL_NUM);
	val =  (EVAL_MAX_DIST-dist)/100;	

	return GetEvalResult()+val;
}


// BoardEvent ��Ʈ��ó�� �� �Լ�.
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
	// ��������� �Ÿ��� ������ �ִ밪���� �����Ѵ�.
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

// ���� ��ġ�� �ʱ�ȭ �Ѵ�.
void CBoard::InitBallPos()
{
	m_ball[0].SetXY(_BOARD_WIDTH*2/8,_BOARD_HEIGHT/2+BALL_DIAMETER+2);
	m_ball[1].SetXY(_BOARD_WIDTH*7/8,_BOARD_HEIGHT/2);
	m_ball[2].SetXY(_BOARD_WIDTH*2/8,_BOARD_HEIGHT/2);
	m_ball[3].SetXY(_BOARD_WIDTH*6/8,_BOARD_HEIGHT/2);
}

// �ε�ĥ������ �Ҹ��� ������ ��ȭ ��Ų��. 
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