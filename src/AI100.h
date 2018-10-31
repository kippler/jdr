// AI100.h: interface for the CAI100 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI100_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
#define AFX_AI100_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AI30.h"

class CBoardView;

class CAI100 : public CAI30  
{
public:
	void SetParent(CBoardView *pParent);
	void AddFirstCourse();
	int GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power);
	bool Do();
	CAI100();
	virtual ~CAI100();

protected :
	CStrokeArrayData	m_arrSD;		// array of stroke data!
	int					m_nErrorFactor;	// 숫자가 클수록 칠때의 에러가 적다.
	CBoardView			*m_pParent;
protected:
	double GetDist(double x1, double y1, double x2, double y2);
	void  AddAimRedBallCourse();
	void SelectTop20();
	void ExpandNode();
};

#endif // !defined(AFX_AI100_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
