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

#define MAX_BALL_V0_SPEED		15000	// 공의 최고 초속도 6000 mm/sec

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

// ball.ini 에서 읽어올 상수를 저장할 스트럭쳐
struct BallConstant {
	int nV2R;			// 속도가 회전에 미치는 영향
	int nR2R;			// 회전이 회전에 미치는 영향
	int nR2V;			// 회전이 속도에 미치는 영향
	int nV2V;			// 속도가 감속 
	int nRz2Rz;			// Rz회전 속도 감소 정도.

	int nP2R;			// 때리는 힘이 회전에 미치는 영향		(역수)
	int nY2R;			// 때리는 위치(by) 가 회전에 미치는 영향(역수)
	int nX2Rz;			// 때리는 위치(bx) 가 회전(Rz)에 미치는 영향

	int nRz2V;			// 벽에 충돌시 회전(Rz)가 속도에 미치는 영향
	int nV2Rz;			// 벽에 충돌시 속도가 회전(Rz)에  미치는 영향

	int nA2A;			// 가속도의 감소율(=마찰계수)

	int nS2S;			// 벽에 충돌시 속도 감소율(%)
	int nBR2BR;			// 공끼리 충돌시 회전력의 전달율(%)
};

class CBall  
{
protected :
	double m_x, m_y;				// 좌표.(mm)
	double m_ax,m_ay;				// 가속도.(mm/s^2)
	double m_vx,m_vy;				// 속도 (mm/s)
	double m_rx,m_ry,m_rz;			// 각속도 (radian)
	double m_e;						// 탄성계수

	double m_width, m_height;		// Board 의 크기.

	double m_pntAlpha, m_pntBeta;	// 공에 찍혀 있는 점의 위치 (라디안),
	double m_pntX,m_pntY,m_pntZ;	// 공의 3차원 좌표.

	int m_num;						// 일련번호.
	static BallConstant	m_cnt;			// 각종 상수 저장 
	CBoard* m_pParent;				// Board 의 pointer.

public :
	static int m_factoryNum;		// 공장 일련번호?
	bool	m_bBlockToWall;			// 쫌전에 벽에 충돌했나? 

public:
	bool IsStop();
	void Stop();
	bool ReadConstant();			// 상수(m_cnt) 읽기 함수.
	void Collision(const CBall& destBall);
	// bx,by : ball 때리는 위치   dx,dy : 때리는 방향     power : 강도 
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

	// 위에서 본 공의 위치. 보이면 true 안보이면(밑에 있으면) false
	bool GetPoint(double &x, double &y);

};

#endif // !defined(AFX_BALL_H__BAF3FEA2_11AE_11D2_B8EB_E814512230CE__INCLUDED_)
