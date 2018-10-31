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

#define		MAX_INPUT_NODE		8			// �Է� ����Ÿ �� - ��4�� = 8��
#define		MAX_OUT_NODE		5			// ��� ����Ÿ �� - ����(2), ��ġ(2), ��(1)


class CAIBpn  : public CAI30  
{
public:
	CBpn		*m_pBpn;
	CAIBpn();
	CDblArray	m_arrIn, m_arrOut, m_arrTarget;		// BPN�� ���� �Ķ��Ÿ��.
	CString		m_szFileName;

	double		m_MinError;							// ���� ������ �ּҰ� ����.
	double		m_CurError;							// ���� ���� ��� ���.

public:
	virtual ~CAIBpn();
	int GetResult(int & n, double & bx, double & by, double & dx, double & dy, double & power);
	bool Do();

private:
	void Normalization();
};


#endif // !defined(AFX_AIBPN_H__7CF320A1_690F_11D2_8984_00AA00BFD4FC__INCLUDED_)
