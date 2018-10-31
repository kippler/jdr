// JDRView.h : interface of the CJDRView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_JDRVIEW_H__0EBE982E_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
#define AFX_JDRVIEW_H__0EBE982E_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Board.h"
#include "BallInput.h"


#define	JDR_VIEW_WIDTH		700
#define JDR_VIEW_HEIGHT		500


class CJDRView : public CView
{
protected: // create from serialization only
	CJDRView();
	DECLARE_DYNCREATE(CJDRView)

// Attributes
public:
	CBallInput		*m_pBallInputWnd;
	CSliderCtrl		m_sliderCtrl;
	CButton			m_buttonFire;
	CEdit			m_EditCtrl;
	CEdit			m_EditChat;

	int			m_nBallNum;		// 공번호
	double		m_dBx,m_dBy;	// 때리는 위치
	double		m_dDx,m_dDy;	// 방향
	double		m_dPower;		// 힘.

	int			m_nPlayerNum;	// 플레이어 일련번호. 


private :
	CBitmap		*m_pOldBitmapBackGnd;
	CDC			m_dcMEMBackGnd;	// 다이 그림을 저장할 DC
	CBitmap		m_bmBackGnd;	// 다이 그림을 가지고 있을 BITMAP

// Operations
public:
	CJDRDoc*		GetDocument();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJDRView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SendStrokeDataToNetwork(const StrokeStatus &ss);
	void TransChatData();
	virtual ~CJDRView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJDRView)
	afx_msg void OnDestroy();
	afx_msg void OnNethost();
	afx_msg void OnNetconnect();
	afx_msg void OnNetdisconnect();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateNetconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNetdisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNethost(CCmdUI* pCmdUI);
	afx_msg LONG OnReceiveData(UINT, LONG);
	afx_msg void OnSetPlayer1();
	afx_msg void OnSetPlayer2();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OnButtonFire();
};

#ifndef _DEBUG  // debug version in JDRView.cpp
inline CJDRDoc* CJDRView::GetDocument()
   { return (CJDRDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JDRVIEW_H__0EBE982E_D16C_11D1_947D_B0FDEA7AB860__INCLUDED_)
