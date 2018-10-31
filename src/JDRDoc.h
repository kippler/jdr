// JDRDoc.h : interface of the CJDRDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_JDRDOC_H__0EBE982C_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
#define AFX_JDRDOC_H__0EBE982C_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_

#include "BoardView.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CJDRView;

class CJDRDoc : public CDocument
{
protected: // create from serialization only
	CJDRDoc();
	DECLARE_DYNCREATE(CJDRDoc)

// Attributes
public:
	CBoardView* m_pBoardView;
	CJDRView*	m_pView;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJDRDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJDRDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJDRDoc)
	afx_msg void OnTest();
	afx_msg void OnTest2();
	afx_msg void OnReadConstant();
	afx_msg void OnMemoryBallPos();
	afx_msg void OnRememberBallPos();
	afx_msg void OnAi30();
	afx_msg void OnAi50();
	afx_msg void OnSpeedTest();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnAiBpn();
	afx_msg void OnAi80();
	afx_msg void OnAi100();
	afx_msg void OnAi150();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JDRDOC_H__0EBE982C_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
