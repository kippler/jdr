// JDR.h : main header file for the JDR application
//

#if !defined(AFX_JDR_H__0EBE9826_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
#define AFX_JDR_H__0EBE9826_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

void _cdecl msg(const char* str, ...);//본체는 JDRView.cpp에..


class CBoardView;
/////////////////////////////////////////////////////////////////////////////
// CJDRApp:
// See JDR.cpp for the implementation of this class
//

class CJDRApp : public CWinApp
{
public:
	CJDRApp();

private :
	CBoardView* m_pBoardView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJDRApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJDRApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JDR_H__0EBE9826_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
