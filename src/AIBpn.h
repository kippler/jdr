// AIBpn.h: interface for the CAIBpn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIBPN_H__7CF320A1_690F_11D2_8984_00AA00BFD4FC__INCLUDED_)
#define AFX_AIBPN_H__7CF320A1_690F_11D2_8984_00AA00BFD4FC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AI30.h"
#include "Bpn.h"

#define		MAX_INPUT_NODE		8			// 입력 데이타 수 - 공4개 = 8개
#define		MAX_OUT_NODE		5			// 출력 데이타 수 - 방향(2), 위치(2), 힘(1)


class CAIBpn  : public CAI30  
{
public:
	CBpn		*m_pBpn;
	CAIBpn();
	CDblArray	m_arrIn, m_arrOut, m_arrTarget;		// BPN에 보낼 파라메타용.
	CString		m_szFileName;

	double		m_MinError;							// 현재 에러중 최소값 저장.
	double		m_CurError;							// 최종 에러 계산 결과.

public:
	virtual ~CAIBpn();
	int GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power);
	bool Do();

private:
	void Normalization();
};


#endif // !defined(AFX_AIBPN_H__7CF320A1_690F_11D2_8984_00AA00BFD4FC__INCLUDED_)
