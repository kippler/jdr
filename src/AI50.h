// AI50.h: interface for the CAI50 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI50_H__5E439261_3B43_11D2_BC35_444553540001__INCLUDED_)
#define AFX_AI50_H__5E439261_3B43_11D2_BC35_444553540001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AI30.h"

class CAI50 : public CAI30  
{
public:
	int GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power);
	bool Do();
	CAI50();
	virtual ~CAI50();

protected :
	CStrokeArrayData	m_arrSD;		// array of stroke data!
protected:
	void AddFirstAldamaCourse();
private:
	void AddRotaionControl();
	void AddPowerControl();
};

#endif // !defined(AFX_AI50_H__5E439261_3B43_11D2_BC35_444553540001__INCLUDED_)
