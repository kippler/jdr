#if !defined(AFX_DLGCHATTING_H__1A39BD84_3BA5_11D2_8146_101A09C10000__INCLUDED_)
#define AFX_DLGCHATTING_H__1A39BD84_3BA5_11D2_8146_101A09C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgChatting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChatting dialog

class CDlgChatting : public CDialog
{
// Construction
public:
	void OnReceive();
	CDlgChatting(CWnd* pParent = NULL);   // standard constructor

	void	OnSend();

	CString		m_strSendMessage;
	CEdit		m_editcViewData;
//	CEdit		m_strViewData;
	CString		m_strViewData;
	CString		m_strTempData;

// Dialog Data
	//{{AFX_DATA(CDlgChatting)
	enum { IDD = IDD_NETCHAT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChatting)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChatting)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHATTING_H__1A39BD84_3BA5_11D2_8146_101A09C10000__INCLUDED_)
