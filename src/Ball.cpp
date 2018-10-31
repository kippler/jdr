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
BallConstant CBall::m_cnt;			// ���� ��� ���� 

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

	// �ӵ��� ���� ���� �̵�.
	m_x +=(m_vx*TIME_SLICE);
	m_y +=(m_vy*TIME_SLICE);

	// ���� �� ǥ�ø� ���� ���
	if(bFast==false) {
		CPoint3 n,o;	// new and old ..
		o.x = m_pntX;
		o.y = m_pntY;
		o.z = m_pntZ;
		// x �� ȸ�� - ry �� �ݴ�.
		n.x = o.x;
		n.y = o.y*cos(-m_ry*TIME_SLICE)-o.z*sin(-m_ry*TIME_SLICE);
		n.z = o.y*sin(-m_ry*TIME_SLICE)+o.z*cos(-m_ry*TIME_SLICE);
		// y �� ȸ�� - rx �� ����
		o = n;
		n.x = o.x*cos(m_rx*TIME_SLICE)+o.z*sin(m_rx*TIME_SLICE);
		n.y = o.y;
		n.z =-o.x*sin(m_rx*TIME_SLICE)+o.z*cos(m_rx*TIME_SLICE);
		// z�� ȸ�� - rz �� ����
		o = n;
		n.x = o.x*cos(m_rz*TIME_SLICE)-o.y*sin(m_rz*TIME_SLICE);
		n.y = o.x*sin(m_rz*TIME_SLICE)+o.y*cos(m_rz*TIME_SLICE);
		n.z = o.z;
		m_pntX = n.x;
		m_pntY = n.y;
		m_pntZ = n.z;
	}


	// ���� �浹�� �ӵ� ����
	if(m_x-BALL_RADIUS<0||m_x+BALL_RADIUS>m_width||m_y-BALL_RADIUS<0||m_y+BALL_RADIUS>m_height)	{
		double theta;	// ���� ���� ����.
		double v;		// ���� �ӵ� 
		theta = atan2(m_vy,m_vx);
		v = sqrt(m_vx*m_vx+m_vy*m_vy);
		v = v*m_cnt.nS2S/100;
		m_vx = cos(theta)*v;
		m_vy = sin(theta)*v;
		m_bBlockToWall = true;			// ���� �浹�ߴ�!!
	}
	else 
		m_bBlockToWall = false;


	// ���� �浹�� ���� ȸ��(rz) ��ȭ
	double vy,vx; // �ӽ� ����
	if(m_x-BALL_RADIUS<0) {						// ���� ��  (���� ȭ��󿡼�)  
		vy   = m_vy + m_rz*BALL_RADIUS*m_cnt.nRz2V/100;	
		m_rz = m_rz + sqrt(fabs(m_vy/BALL_RADIUS))*m_cnt.nV2Rz/100;	
		m_vy = vy;
	}											
	else if(m_x+BALL_RADIUS>m_width)	{				// �����ʺ� 
		vy   = m_vy - m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz - sqrt(fabs(m_vy/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vy = vy;
	}
	else if(m_y-BALL_RADIUS<0)	{					// ���� ��       
		vx   = m_vx - m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz - sqrt(fabs(m_vx/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vx = vx;
	}
	else if(m_y+BALL_RADIUS>m_height)	{			// �Ʒ��� ��                                      
		vx   = m_vx + m_rz*BALL_RADIUS*m_cnt.nRz2V/100;
		m_rz = m_rz + sqrt(fabs(m_vx/BALL_RADIUS))*m_cnt.nV2Rz/100;
		m_vx = vx;
	}


	// ���� �浹�� ����(vx,vy) ����.
	if(m_x-BALL_RADIUS<0)			{m_vx = fabs(m_vx);m_x = BALL_RADIUS;}			// ���� ��  (���� ȭ��󿡼�)
	if(m_x+BALL_RADIUS>m_width)		{m_vx = -fabs(m_vx);m_x = m_width-BALL_RADIUS;}	// �����ʺ�
	if(m_y-BALL_RADIUS<0)			{m_vy = fabs(m_vy);m_y = BALL_RADIUS;}			// ���� ��
	if(m_y+BALL_RADIUS>m_height)	{m_vy = -fabs(m_vy);m_y = m_height-BALL_RADIUS;}// �Ʒ��� ��

	// ���ӵ� ����  -> ���� ����
	if(m_vx||m_vy) {
		double v = sqrt(m_vx*m_vx+m_vy*m_vy);
		double ax = fabs(m_vx/v)*m_cnt.nA2A;
		double ay = fabs(m_vy/v)*m_cnt.nA2A;

		m_ax = (m_vx>=0) ? -ax : ax;
		m_ay = (m_vy>=0) ? -ay : ay;
	}

	// ���ӵ��� ���� �ӵ��� ��ȭ -> ���� ����
	if(m_vx>=0)	m_vx = max(0,m_vx+m_ax*TIME_SLICE);
	else        m_vx = min(0,m_vx+m_ax*TIME_SLICE);

	if(m_vy>=0)	m_vy = max(0,m_vy+m_ay*TIME_SLICE);
	else        m_vy = min(0,m_vy+m_ay*TIME_SLICE);


	// ���� ȸ���ӵ��� ���� �ӵ����� ���� ����ϰ� ����� -> ��� ���� �ʿ�.
	// alpha blending(?) �̿��Ѵ�.

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

	m_rx = (dx*power/m_cnt.nP2R)*by/m_cnt.nY2R*100;		// ���� ��� : ������ ���� ȸ���� ��ġ�� ����
	m_ry = (dy*power/m_cnt.nP2R)*by/m_cnt.nY2R*100;		// ������ ��� : ������ ������ ȸ���� ��ġ�� ����
//	m_rx = dx*power*m_cnt.nP2R/100 + dx*by*m_cnt.nY2R/100;
//	m_ry = dy*power*m_cnt.nP2R/100 + dx*by*m_cnt.nY2R/100;

	// �̺κ��� ���߿� ����� ���� �Ѵ�.
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

// destBall �� ���� �浹�� ����
void CBall::Collision(const CBall &destBall)
{
	// ��� �ӵ� ���ϱ�
	double vx = m_vx - destBall.m_vx;
	double vy = m_vy - destBall.m_vy;
	if(vx==0&&vy==0) return;

	// ���� ���� ��� ���� ����.
	double alpha = atan2(vy,vx);

	// ���� ��� ���� ���⿡ ���� ������ ���� ���ϱ�.
	double x = destBall.m_x - m_x;
	double y = destBall.m_y - m_y;
	double theta = atan2(y,x)-alpha;

	// �浹�� ���� ���� ȸ��. (�����..)
	alpha = alpha - (M_PI_2-theta);
	
	// ����� ���� �ӵ�.
	double v = sqrt(vx*vx + vy*vy);

	// �浹�� ����� ���� �ӵ�.
	v = v*sin(theta);

	// ����� ���� �ӵ� 
	vx = v * cos(alpha);
	vy = v * sin(alpha);

	// ������ ���� �ӵ�.
	vx = vx + destBall.m_vx;
	vy = vy + destBall.m_vy;

	// ��� ������ �ݿ�.
	m_vx = vx;
	m_vy = vy;


	/* ���� ���� �浹 �� ������ ����߸���.*/
	x = m_x - destBall.m_x;
	y = m_y - destBall.m_y;
	// ��� ������ �� ������ ����.
	theta = atan2(y,x);

	// ���� �̵����� ������.
	m_x = cos(theta)*(BALL_DIAMETER/2*1.001) + (destBall.m_x+m_x)/2;
	m_y = sin(theta)*(BALL_DIAMETER/2*1.001) + (destBall.m_y+m_y)/2;

	// ȸ�� ��� ����.
	m_rx = (m_rx*(100-m_cnt.nBR2BR) - destBall.m_rx*m_cnt.nBR2BR)/100;
	m_ry = (m_ry*(100-m_cnt.nBR2BR) - destBall.m_ry*m_cnt.nBR2BR)/100;
	m_rz = (m_rz*(100-m_cnt.nBR2BR) - destBall.m_rz*m_cnt.nBR2BR)/100;

}

/*
struct BallConstant {
	int nV2R;			// �ӵ��� ȸ���� ��ġ�� ����
	int nR2R;			// ȸ���� ȸ���� ��ġ�� ����
	int nR2V;			// ȸ���� �ӵ��� ��ġ�� ����
	int nV2V;			// �ӵ��� ���� 
	int nRz2Rz;			// Rzȸ�� �ӵ� ���� ����.

	int nP2R;			// ������ ���� ȸ���� ��ġ�� ����		(����)
	int nY2R;			// ������ ��ġ(by) �� ȸ���� ��ġ�� ����(����)
};
*/

// ���������� ���� ����� ball.ini ���� �о� ���δ�.

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

// ���� �����.
void CBall::Stop()
{
	m_vx = m_vy = m_rx = m_ry = m_rz = m_ax = m_ay = 0;
}

// ���� ���� ���� �ֳ�?
bool CBall::IsStop()
{
	if( int(m_vx*100)==0&&
		int(m_vy*100)==0) return true;
	return false;

//	if(m_vx||m_vy||m_rx||m_ry||m_rz||m_ax||m_ay) return false;
//	return true;
}
