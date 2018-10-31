// AI80.h: interface for the CAI80 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI80_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
#define AFX_AI80_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AI30.h"

class CAI80 : public CAI30  
{
public:
	int GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power);
	bool Do();
	CAI80();
	virtual ~CAI80();

protected :
	CStrokeArrayData	m_arrSD;		// array of stroke data!
protected:
	void AddFirstAldamaCourse();
private:
	double GetDist(double x1, double y1, double x2, double y2);
private:
	void AddRotaionControl();
	void AddPowerControl();
};

#endif // !defined(AFX_AI80_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
