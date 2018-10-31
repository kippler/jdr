// Ball.cpp: implementation of the CBall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDR.h"
#include "Ball.h"
#include "Board.h"
#include <math.h>
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CBall::m_factoryNum = 0;
BallConstant CBall::m_cnt;			// 각종 상수 저장 

CBall::CBall()  
{
	m_pParent = NULL;
	m_x = m_y = m_ax = m_ay = m_width = m_height = 0;	
	m_vx = m_vy = m_rx = m_ry = m_rz = 0;
	m_e = 0;

	m_pntAlpha = m_pntBeta = 0;
	m_pntX = m_pntY = 0;
	m_pntZ = BALL_RADIUS;

	m_num = m_factoryNum++;

	m_bBlockToWall = false;

	ReadConstant();
}

void CBall::SetParent(CBoard* pBoardParent)
{
	m_pParent = pBoardParent;
	m_width = m_pParent->GetWidth();
	m_height = m_pParent->GetHeight();

}

CBall::~CBall()
{

}

void CBall::DoMove(bool bFast)
{

	// 속도에 따른 공의 이동.
	m_x +=(m_vx*TIME_SLICE);
	m_y +=(m_vy*TIME_SLICE);

	// 공의 점 표시를 위한 계산
	if(bFast==false) {
		CPoint3 n,o;	// new and old ..
		o.x = m_pntX;
		o.y = m_pntY;
		o.z = m_pntZ;
		// x 축 회전 - ry 와 반대.
		n.x = o.x;
		n.y = o.y*cos(-m_ry*TIME_SLICE)-o.z*sin(-m_ry*TIME_SLICE);
		n.z = o.y*sin(-m_ry*TIME_SLICE)+o.z*cos(-m_ry*TIME_SLICE);
		// y 축 회전 - rx 와 동일
		o = n;
		n.x = o.x*cos(m_rx*TIME_SLICE)+o.z*sin(m_rx*TIME_SLICE);
		n.y = o.y;
		n.z =-o.x*sin(m_rx*TIME_SLICE)+o.z*cos(m_rx*TIME_SLICE);
		// z축 회전 - rz 와 동일
		o = n;
		n.x = o.x*cos(m_rz*TIME_SLICE)-o.y*sin(m_rz*TIME_SLICE);
		n.y = o.x*sin(m_rz*TIME_SLICE)+o.y*cos(m_rz*TIME_SLICE);
		n.z = o.z;
		m_pntX = n.x;
		m_pntY = n.y;
		m_pntZ = n.z;
	}


	// 벽에 충돌시 속도 감소
	if(m_x-BALL_RADIUS<0||m_x+BALL_RADIUS>m_width||m_y-BALL_RADIUS<0||m_y+BALL_RADIUS>m_height)	{
		double theta;	// 공의 진행 방향.
		double v;		// 공의 속도 
		theta = atan2(m_vy,m_vx);
		v = sqrt(m_vx*m_vx+m_vy*m_vy);
		v = v*m_cnt.nS2S/100;
		m_vx = cos(theta)*v;
		m_vy = sin(theta)*v;
		m_bBlockToWall = true;			// 벽에 충돌했다!!
	}
	else 
		m_bBlockToWall = false;


	// 벽에 충돌시 공의 회전(rz) 변화
	double vy,vx; // 임시 변수
	if(m_x-BALL_RADIUS<0) {						// 왼쪽 벽  (전부 화면상에서)  
		vy   = m_vy + m_rz*BALL_RADIUS*m_cnt.nRz2V/100;	
		m_rz = m_rz + sqrt(fabs(m_vy/BALL_RADIUS))*m_cnt.nV2Rz/100;	
		m_vy = vy;
	}											
	else if(m_x+BALL_RADIUS>m_width)	{				// 오른쪽벽 
		vy   = m_vy - m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz - sqrt(fabs(m_vy/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vy = vy;
	}
	else if(m_y-BALL_RADIUS<0)	{					// 위쪽 벽       
		vx   = m_vx - m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz - sqrt(fabs(m_vx/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vx = vx;
	}
	else if(m_y+BALL_RADIUS>m_height)	{			// 아래쪽 벽                                      
		vx   = m_vx + m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz + sqrt(fabs(m_vx/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vx = vx;
	}


	// 벽에 충돌시 방향(vx,vy) 변경.
	if(m_x-BALL_RADIUS<0)			{m_vx = fabs(m_vx);m_x = BALL_RADIUS;}			// 왼쪽 벽  (전부 화면상에서)
	if(m_x+BALL_RADIUS>m_width)		{m_vx = -fabs(m_vx);m_x = m_width-BALL_RADIUS;}	// 오른쪽벽
	if(m_y-BALL_RADIUS<0)			{m_vy = fabs(m_vy);m_y = BALL_RADIUS;}			// 위쪽 벽
	if(m_y+BALL_RADIUS>m_height)	{m_vy = -fabs(m_vy);m_y = m_height-BALL_RADIUS;}// 아래쪽 벽

	// 가속도 설정  -> 수정 예정
	if(m_vx||m_vy) {
		double v = sqrt(m_vx*m_vx+m_vy*m_vy);
		double ax = fabs(m_vx/v)*m_cnt.nA2A;
		double ay = fabs(m_vy/v)*m_cnt.nA2A;

		m_ax = (m_vx>=0) ? -ax : ax;
		m_ay = (m_vy>=0) ? -ay : ay;
	}

	// 가속도에 따른 속도의 변화 -> 수정 예정
	if(m_vx>=0)	m_vx = max(0,m_vx+m_ax*TIME_SLICE);
	else        m_vx = min(0,m_vx+m_ax*TIME_SLICE);

	if(m_vy>=0)	m_vy = max(0,m_vy+m_ay*TIME_SLICE);
	else        m_vy = min(0,m_vy+m_ay*TIME_SLICE);


	// 공의 회전속도와 공의 속도차를 서로 비슷하게 만들다 -> 계수 수정 필요.
	// alpha blending(?) 이용한다.

	double new_rx, new_ry;
	new_rx = (m_vx/BALL_RADIUS*m_cnt.nV2R*0.01 + m_rx*m_cnt.nR2R*0.01)/100;
	new_ry = (m_vy/BALL_RADIUS*m_cnt.nV2R*0.01 + m_ry*m_cnt.nR2R*0.01)/100;
	m_vx   = (m_rx*BALL_RADIUS*m_cnt.nR2V*0.01 + m_vx*m_cnt.nV2V*0.01)/100;
	m_vy   = (m_ry*BALL_RADIUS*m_cnt.nR2V*0.01 + m_vy*m_cnt.nV2V*0.01)/100;
	m_rx = new_rx;
	m_ry = new_ry;

	m_rz = (m_rz==0) ? 0: (m_rz>0)? m_rz=max(0,m_rz-m_cnt.nRz2Rz*TIME_SLICE) : m_rz=min(0,m_rz+m_cnt.nRz2Rz*TIME_SLICE);


	m_x = min(max(m_x, BALL_RADIUS+1), m_width - BALL_RADIUS-1);
	m_y = min(max(m_y, BALL_RADIUS+1), m_height- BALL_RADIUS-1);

}

void CBall::SetXY(double x, double y)
{
	m_x = x;
	m_y = y;
}
void CBall::SetAXY(double ax, double ay)
{
	m_ax = ax;
	m_ay = ay;
}
void CBall::SetVXY(double vx, double vy)
{
	m_vx = vx;
	m_vy = vy;
}

double CBall::GetX()
{
	return m_x;
}

double CBall::GetY()
{
	return m_y;
}


bool CBall::Stroke(double bx, double by, double dx, double dy, double power)
{
	m_vx = dx*power;
	m_vy = dy*power;

	m_rx = (dx*power/m_cnt.nP2R)*by/m_cnt.nY2R*100;		// 왼쪽 상수 : 때리는 힘이 회전에 미치는 영향
	m_ry = (dy*power/m_cnt.nP2R)*by/m_cnt.nY2R*100;		// 오른쪽 상수 : 때리는 부위가 회전에 미치는 영향
//	m_rx = dx*power*m_cnt.nP2R/100 + dx*by*m_cnt.nY2R/100;
//	m_ry = dy*power*m_cnt.nP2R/100 + dx*by*m_cnt.nY2R/100;

	// 이부분은 나중에 계수가 들어가야 한다.
//	m_rz = bx/BALL_RADIUS*power/BALL_RADIUS;
	m_rz = -bx/BALL_RADIUS*m_cnt.nX2Rz*sqrt(sqrt(sqrt(power)));

	return true;
}

bool CBall::GetPoint(double &x, double &y)
{
	x = m_pntX;
	y = m_pntY;
	return m_pntZ>0 ? true : false;
}

// destBall 은 나와 충돌한 상대공
void CBall::Collision(const CBall &destBall)
{
	// 상대 속도 구하기
	double vx = m_vx - destBall.m_vx;
	double vy = m_vy - destBall.m_vy;
	if(vx==0&&vy==0) return;

	// 현재 공의 상대 진행 각도.
	double alpha = atan2(vy,vx);

	// 공의 상대 진행 방향에 대한 상대공의 각도 구하기.
	double x = destBall.m_x - m_x;
	double y = destBall.m_y - m_y;
	double theta = atan2(y,x)-alpha;

	// 충돌후 현재 공의 회전. (상대적..)
	alpha = alpha - (M_PI_2-theta);
	
	// 상대적 진행 속도.
	double v = sqrt(vx*vx + vy*vy);

	// 충돌후 상대적 진행 속도.
	v = v*sin(theta);

	// 상대적 진행 속도 
	vx = v * cos(alpha);
	vy = v * sin(alpha);

	// 절대적 진행 속도.
	vx = vx + destBall.m_vx;
	vy = vy + destBall.m_vy;

	// 멤버 변수에 반영.
	m_vx = vx;
	m_vy = vy;


	/* 현재 공을 충돌 한 공에서 떨어뜨리기.*/
	x = m_x - destBall.m_x;
	y = m_y - destBall.m_y;
	// 상대 공에서 본 내공의 각도.
	theta = atan2(y,x);

	// 공을 이동시켜 버리기.
	m_x = cos(theta)*(BALL_DIAMETER/2*1.001) + (destBall.m_x+m_x)/2;
	m_y = sin(theta)*(BALL_DIAMETER/2*1.001) + (destBall.m_y+m_y)/2;

	// 회전 운동의 전달.
	m_rx = (m_rx*(100-m_cnt.nBR2BR) - destBall.m_rx*m_cnt.nBR2BR)/100;
	m_ry = (m_ry*(100-m_cnt.nBR2BR) - destBall.m_ry*m_cnt.nBR2BR)/100;
	m_rz = (m_rz*(100-m_cnt.nBR2BR) - destBall.m_rz*m_cnt.nBR2BR)/100;

}

/*
struct BallConstant {
	int nV2R;			// 속도가 회전에 미치는 영향
	int nR2R;			// 회전이 회전에 미치는 영향
	int nR2V;			// 회전이 속도에 미치는 영향
	int nV2V;			// 속도가 감속 
	int nRz2Rz;			// Rz회전 속도 감소 정도.

	int nP2R;			// 때리는 힘이 회전에 미치는 영향		(역수)
	int nY2R;			// 때리는 위치(by) 가 회전에 미치는 영향(역수)
};
*/

// 내부적으로 사용될 상수를 ball.ini 에서 읽어 들인다.

#define BALL_APP_NAME	"BallConstant"
#define	BALL_FILE_NAME  GetThisPath() + "ball.ini"
bool CBall::ReadConstant()
{
	m_cnt.nV2R   = GetPrivateProfileInt(BALL_APP_NAME,"V2R",  0,BALL_FILE_NAME );
	m_cnt.nR2R   = GetPrivateProfileInt(BALL_APP_NAME,"R2R",  0,BALL_FILE_NAME );
	m_cnt.nR2V   = GetPrivateProfileInt(BALL_APP_NAME,"R2V",  0,BALL_FILE_NAME );
	m_cnt.nV2V   = GetPrivateProfileInt(BALL_APP_NAME,"V2V",  0,BALL_FILE_NAME );
	m_cnt.nRz2Rz = GetPrivateProfileInt(BALL_APP_NAME,"Rz2Rz",0,BALL_FILE_NAME );

	m_cnt.nP2R   = GetPrivateProfileInt(BALL_APP_NAME,"P2R",  0,BALL_FILE_NAME );
	m_cnt.nY2R   = GetPrivateProfileInt(BALL_APP_NAME,"Y2R",  0,BALL_FILE_NAME );
	m_cnt.nX2Rz   = GetPrivateProfileInt(BALL_APP_NAME,"X2Rz",0,BALL_FILE_NAME );

	m_cnt.nRz2V  = GetPrivateProfileInt(BALL_APP_NAME,"Rz2V", 0,BALL_FILE_NAME );
	m_cnt.nV2Rz  = GetPrivateProfileInt(BALL_APP_NAME,"V2Rz", 0,BALL_FILE_NAME );

	m_cnt.nA2A   = GetPrivateProfileInt(BALL_APP_NAME,"A2A",  0,BALL_FILE_NAME );
	m_cnt.nS2S   = GetPrivateProfileInt(BALL_APP_NAME,"S2S",  0,BALL_FILE_NAME );
	m_cnt.nBR2BR = GetPrivateProfileInt(BALL_APP_NAME,"BR2BR",0,BALL_FILE_NAME );

//	int d = GetPrivateProfileInt(BALL_APP_NAME,"DEBUG", -1,BALL_FILE_NAME );
//	debug("d:%d",d);

	return true;
}

// 공을 멈춘다.
void CBall::Stop()
{
	m_vx = m_vy = m_rx = m_ry = m_rz = m_ax = m_ay = 0;
}

// 현재 공이 멈춰 있나?
bool CBall::IsStop()
{
	if( int(m_vx*100)==0&&
		int(m_vy*100)==0) return true;
	return false;

//	if(m_vx||m_vy||m_rx||m_ry||m_rz||m_ax||m_ay) return false;
//	return true;
}
