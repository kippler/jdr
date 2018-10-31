// DlgChatting.cpp : implementation file
//

#include "stdafx.h"
#include "jdr.h"
#include "DlgChatting.h"

#include "mainfrm.h"
#include "jdrdoc.h"
#include "jdrview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChatting dialog


CDlgChatting::CDlgChatting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChatting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChatting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgChatting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChatting)
		DDX_Control(pDX, IDC_EDIT2, m_editcViewData);
		DDX_Text (pDX, IDC_EDIT1, m_strSendMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChatting, CDialog)
	//{{AFX_MSG_MAP(CDlgChatting)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChatting message handlers

BOOL CDlgChatting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgChatting::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// ��ȭ���ڿ��� ����Ű�� ������ 
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==13)
	{
		if (GetDlgItem(IDC_EDIT1)->m_hWnd == pMsg->hwnd)
		{
			pMsg->wParam=9;
			OnSend();
		}
		else 
		{
			pMsg->wParam=9;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgChatting::OnSend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);		// ����Ÿ�� �����´�. 

	// �信 ����Ÿ�� ���������� ó�� 
//	CJDRView* pView = (CJDRView*)((CMainFrame*)
//			AfxGetApp()->m_pMainWnd)->GetActiveView();
	CJDRSock	*pSock

	// �����ΰ��� Ŭ���̾�Ʈ�ΰ�� ��ȭ�� ǥ�� 
	if (pView->m_bIsHost)
	{
		// m_strTempData = pView->m_strHostNickName + "> " + m_strSendMessage;
		m_strTempData = "> " + m_strSendMessage;
		m_strSendMessage = m_strTempData;
	}
	else 
	{
		// m_strTempData = pView->m_strClientNickName + "> " + m_strSendMessage;
		m_strTempData = "> " + m_strSendMessage;
		m_strSendMessage = m_strTempData;
	}

	// ���� ȭ�� ���� 
	m_editcViewData.GetWindowText(m_strViewData);
	m_strTempData = m_strViewData;		
	m_strViewData = m_strSendMessage + "\r\n";
	m_strViewData = m_strTempData + m_strViewData;
	m_editcViewData.SetWindowText(m_strViewData);

	// ����Ÿ�� ��ȭ���ڷ� �ø���. 
	UpdateData(false);		

	// ��ũ���� ���� SetSel�̿� 
	m_editcViewData.SetSel(m_strViewData.GetLength(),
									m_strViewData.GetLength());

	pView->m_strSendData = m_strSendMessage;
//	pView->OnSend();

	m_strSendMessage.Empty();
	UpdateData(false);
}

void CDlgChatting::OnReceive()
{
	UpdateData(true);		// ����Ÿ�� �����´�. 

	m_editcViewData.GetWindowText(m_strViewData);
	m_strViewData = m_strViewData + m_strTempData + "\r\n";
	m_editcViewData.SetWindowText(m_strViewData);

	UpdateData(false);		// ����Ÿ�� ��ȭ���ڷ� �ø���.

	// ��ũ���� ���� SetSel�̿� 
	m_editcViewData.SetSel(m_strViewData.GetLength(),
									m_strViewData.GetLength());
}

void CDlgChatting::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
/*
	// �信 ����Ÿ�� ���������� ó�� 
	CNettest2View* pView = (CNettest2View*)((CMainFrame*)
			AfxGetApp()->m_pMainWnd)->GetActiveView();

	// ������ �ݱ����� view�� �Լ� ȣ��...
	if (pView->m_bIsHost || pView->m_bConnectClient)
	{
		pView->CloseSocket();
	}

	delete pView;
*/
	CJDRView* pView = (CJDRView*)((CMainFrame*)
		AfxGetApp()->m_pMainWnd)->GetActiveView();
	
	pView->m_bChat = false;

	CDialog::OnClose();
}
