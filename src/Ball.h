////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// ball.h
/// 
/// @author   parkkh
/// @date     Monday, January 17, 2011  11:48:03 AM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_BALL_H__BAF3FEA2_11AE_11D2_B8EB_E814512230CE__INCLUDED_)
#define AFX_BALL_H__BAF3FEA2_11AE_11D2_B8EB_E814512230CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#define TIME_GAP	10					// 10 milli sec
//#define TIME_SLICE	0.01				//(TIME_GAP/1000)		// 0.01 
#define TIME_GAP	3					// 3 milli sec
#define TIME_SLICE	0.003				//(TIME_GAP/1000)		// 0.003

#define MAX_BALL_V0_SPEED		15000	// ���� �ְ� �ʼӵ� 6000 mm/sec

#ifndef BALL_DIAMETER
	#define BALL_DIAMETER		65.5
	#define BALL_RADIUS	(BALL_DIAMETER/2)
#endif

class CBoard;

class CPoint3 
{
public :
	double x,y,z;
	CPoint3() {x=y=z=0;}
};

// ball.ini ���� �о�� ����� ������ ��Ʈ����
struct BallConstant {
	int nV2R;			// �ӵ��� ȸ���� ��ġ�� ����
	int nR2R;			// ȸ���� ȸ���� ��ġ�� ����
	int nR2V;			// ȸ���� �ӵ��� ��ġ�� ����
	int nV2V;			// �ӵ��� ���� 
	int nRz2Rz;			// Rzȸ�� �ӵ� ���� ����.

	int nP2R;			// ������ ���� ȸ���� ��ġ�� ����		(����)
	int nY2R;			// ������ ��ġ(by) �� ȸ���� ��ġ�� ����(����)
	int nX2Rz;			// ������ ��ġ(bx) �� ȸ��(Rz)�� ��ġ�� ����

	int nRz2V;			// ���� �浹�� ȸ��(Rz)�� �ӵ��� ��ġ�� ����
	int nV2Rz;			// ���� �浹�� �ӵ��� ȸ��(Rz)��  ��ġ�� ����

	int nA2A;			// ���ӵ��� ������(=�������)

	int nS2S;			// ���� �浹�� �ӵ� ������(%)
	int nBR2BR;			// ������ �浹�� ȸ������ ������(%)
};

class CBall  
{
protected :
	double m_x, m_y;				// ��ǥ.(mm)
	double m_ax,m_ay;				// ���ӵ�.(mm/s^2)
	double m_vx,m_vy;				// �ӵ� (mm/s)
	double m_rx,m_ry,m_rz;			// ���ӵ� (radian)
	double m_e;						// ź�����

	double m_width, m_height;		// Board �� ũ��.

	double m_pntAlpha, m_pntBeta;	// ���� ���� �ִ� ���� ��ġ (����),
	double m_pntX,m_pntY,m_pntZ;	// ���� 3���� ��ǥ.

	int m_num;						// �Ϸù�ȣ.
	static BallConstant	m_cnt;			// ���� ��� ���� 
	CBoard* m_pParent;				// Board �� pointer.

public :
	static int m_factoryNum;		// ���� �Ϸù�ȣ?
	bool	m_bBlockToWall;			// ������ ���� �浹�߳�? 

public:
	bool IsStop();
	void Stop();
	bool ReadConstant();			// ���(m_cnt) �б� �Լ�.
	void Collision(const CBall& destBall);
	// bx,by : ball ������ ��ġ   dx,dy : ������ ����     power : ���� 
	bool Stroke(double bx, double by, double dx, double dy, double power);
	void SetVXY(double vx, double vy);
	double GetY();
	double GetX();
	void SetAXY(double ax, double ay);
	void SetXY(double x, double y);
	void DoMove(bool bFast=false);

	CBall();
	virtual ~CBall();
	void SetParent(CBoard* pBoardParent);

	// ������ �� ���� ��ġ. ���̸� true �Ⱥ��̸�(�ؿ� ������) false
	bool GetPoint(double &x, double &y);

};

#endif // !defined(AFX_BALL_H__BAF3FEA2_11AE_11D2_B8EB_E814512230CE__INCLUDED_)
