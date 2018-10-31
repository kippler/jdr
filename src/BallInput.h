#if !defined(AFX_BALLINPUT_H__70166381_1E53_11D2_8142_08002BE5A1AE__INCLUDED_)
#define AFX_BALLINPUT_H__70166381_1E53_11D2_8142_08002BE5A1AE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BallInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBallInput window

class CBallInput : public CWnd
{
private :
	double	m_x;
	double  m_y;
// Construction
public:
	CBallInput();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBallInput)
	//}}AFX_VIRTUAL

// Implementation
public:
	double GetBX(){return m_x;}
	double GetBY(){return m_y;}
	void GetXY(double &x, double&y);
	CPoint GetXY();
	bool CreateWnd(CWnd * pParent, int x, int y);
	virtual ~CBallInput();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBallInput)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BALLINPUT_H__70166381_1E53_11D2_8142_08002BE5A1AE__INCLUDED_)
