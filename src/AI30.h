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

// ���� ������ ���� ����.
struct STROKE_DATA {
	int		nBallNum;		// �ڽ��� ���� ��ȣ.
	double	bx;				// ���� ���� ����
	double	by;				//       ..
	double	dx;				// ������ ����	
	double	dy;				//       ..
	double	power;			// ������ ��.
	int		nEvalValue;		// �� ��.
};



typedef CArray<STROKE_DATA,STROKE_DATA>	CStrokeArrayData;		// stroke_data : board.h �� ����

class CAI30  
{
protected :
	int m_nBallNum;				// �ڽ��� ���� ��ȣ.
	CBoard *m_pBoard;			// CBoardView���� ������ �ִ� Board�� ������.

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
