// AI150.h: interface for the CAI150 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI150_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
#define AFX_AI150_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AI100.h"

// AI100을 상속받아서 가야시를 볼수 있게 추가한 클래스.
class CAI150 : public CAI100  
{
public:
	void SelectTop10();
	bool Do();
	CAI150();
	virtual ~CAI150();
protected:
	void CheckGayasiCourse();
};

#endif // !defined(AFX_AI150_H__5E439261_3B43_11D2_BC35_444553540002__INCLUDED_)
