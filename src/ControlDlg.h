#if !defined(AFX_CONTROLDLG_H__875DA901_158D_11D2_B8EB_EDEF67D26B53__INCLUDED_)
#define AFX_CONTROLDLG_H__875DA901_158D_11D2_B8EB_EDEF67D26B53__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ControlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlDlg dialog
#include "BallInput.h"

class CBoardView;
class CBallInput;
class CControlDlg : public CDialog
{
private :
	CBoardView	*m_pParent;
	CBallInput	*m_pBallInputWnd;

	int			m_nBallNum;		// 공번호
	double		m_dBx,m_dBy;	// 때리는 위치
	double		m_dDx,m_dDy;	// 방향
	double		m_dPower;		// 힘.

// Construction
public:
	CControlDlg(CWnd* pParent = NULL);   // standard constructor

public :
	 ~CControlDlg();
	double		GetPower() { return m_sldPower.GetPos() ; }
	double		GetBX() {m_pBallInputWnd->GetXY(m_dBx,m_dBy); return m_dBx; }
	double		GetBY() {m_pBallInputWnd->GetXY(m_dBx,m_dBy); return m_dBy; }


// Dialog Data
	//{{AFX_DATA(CControlDlg)
	enum { IDD = IDD_CONTROL_DIALOG };
	CSliderCtrl	m_sldPower;
//	CSliderCtrl	m_sldDx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CControlDlg)
	afx_msg void OnButtonTest();
	afx_msg void OnStaticClick(WPARAM wParam,LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLDLG_H__875DA901_158D_11D2_B8EB_EDEF67D26B53__INCLUDED_)
