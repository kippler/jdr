// AI30.h: interface for the CAI30 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI30_H__A93F4D61_3A92_11D2_A56F_855EC33D7131__INCLUDED_)
#define AFX_AI30_H__A93F4D61_3A92_11D2_A56F_855EC33D7131__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

class CBoard;

// 공을 때리는 정보 저장.
struct STROKE_DATA {
	int		nBallNum;		// 자신의 공의 번호.
	double	bx;				// 때릴 공의 부위
	double	by;				//       ..
	double	dx;				// 때리는 방향	
	double	dy;				//       ..
	double	power;			// 때리는 힘.
	int		nEvalValue;		// 평가 값.
};



typedef CArray<STROKE_DATA,STROKE_DATA>	CStrokeArrayData;		// stroke_data : board.h 에 정의

class CAI30  
{
protected :
	int m_nBallNum;				// 자신의 공의 번호.
	CBoard *m_pBoard;			// CBoardView에서 가지고 있는 Board의 포인터.

	STROKE_DATA	m_strokeData;	// ....

public:
	int  GetResult(int &n, double &bx, double &by, double &dx, double &dy, double &power);
	bool Do();
	void SetBallNum(int nBallNum);
	void SetBoard(CBoard* pBoard);
	CAI30();
	virtual ~CAI30();

};

#endif // !defined(AFX_AI30_H__A93F4D61_3A92_11D2_A56F_855EC33D7131__INCLUDED_)
