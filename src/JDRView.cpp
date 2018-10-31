// JDRView.cpp : implementation of the CJDRView class
//

#include "stdafx.h"
#include "JDR.h"

#include "JDRDoc.h"
#include "JDRView.h"
#include "Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// msg 함수를 위한 전역 변수.
CEdit* pEditCtrl = NULL;

void _cdecl msg(const char* str, ...)
{
	if(pEditCtrl==NULL) return;

	va_list args;
	va_start(args, str);

	int nBuf;
	TCHAR szBuffer[512];
	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer), str, args);

	// was there an error? was the expanded string too long?
	ASSERT(nBuf >= 0);

//	strcat(szBuffer,"\r\n");
	pEditCtrl->SetSel(10000000,10000000);
	pEditCtrl->ReplaceSel(CString("\r\n")+CString(szBuffer));
	//pRichEditCtrl->LineScroll(1);
	va_end(args);
}


/////////////////////////////////////////////////////////////////////////////
// CJDRView

#define IDC_BUTTON_FIRE		11
#define IDC_SLIDER_POWER	10
#define IDC_RICH_EDIT		12
#define IDC_CHAT_EDIT		999



IMPLEMENT_DYNCREATE(CJDRView, CView)

BEGIN_MESSAGE_MAP(CJDRView, CView)
	//{{AFX_MSG_MAP(CJDRView)
	ON_WM_DESTROY()
	ON_COMMAND(ID_NETHOST, OnNethost)
	ON_COMMAND(ID_NETCONNECT, OnNetconnect)
	ON_COMMAND(ID_NETDISCONNECT, OnNetdisconnect)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_NETCONNECT, OnUpdateNetconnect)
	ON_UPDATE_COMMAND_UI(ID_NETDISCONNECT, OnUpdateNetdisconnect)
	ON_UPDATE_COMMAND_UI(ID_NETHOST, OnUpdateNethost)
//	ON_MESSAGE(WM_USER_RECEIVE_DATA, OnReceiveData)
	ON_COMMAND(IDM_SET_PLAYER_1, OnSetPlayer1)
	ON_COMMAND(IDM_SET_PLAYER_2, OnSetPlayer2)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_BUTTON_FIRE, CJDRView::OnButtonFire)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJDRView construction/destruction

CJDRView::CJDRView()
{
	m_nBallNum = 0;
	m_nPlayerNum = 0;

}

CJDRView::~CJDRView()
{
	// 배경 DC삭제.
    m_dcMEMBackGnd.SelectObject(m_pOldBitmapBackGnd);

	delete m_pBallInputWnd;
}

BOOL CJDRView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CJDRView drawing

void CJDRView::OnDraw(CDC* pDC)
{
	CJDRDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

}

/////////////////////////////////////////////////////////////////////////////
// CJDRView printing

BOOL CJDRView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CJDRView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CJDRView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CJDRView diagnostics

#ifdef _DEBUG
void CJDRView::AssertValid() const
{
	CView::AssertValid();
}

void CJDRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CJDRDoc* CJDRView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CJDRDoc)));
	return (CJDRDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJDRView message handlers


#define	SLIDER_TOP		5
#define	SLIDER_LEFT		80
#define SLIDER_WIDTH	200
#define	SLIDER_HEIGHT	20

#define FBUTTON_TOP		30
#define FBUTTON_LEFT	80
#define FBUTTON_WIDTH	40
#define FBUTTON_HEIGHT  40

#define	RE_TOP			5
#define	RE_LEFT			300
#define	RE_WIDTH		335
#define	RE_HEIGHT		90

#define	CHAT_EDIT_TOP		75
#define	CHAT_EDIT_LEFT		5
#define CHAT_EDIT_WIDTH		280
#define CHAT_EDIT_HEIGHT	20

#define BALL_INPUTWND_LEFT	5
#define BALL_INPUTWND_TOP	5

void CJDRView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	// 공의 위치 입력 받는 콘트롤 	
	m_pBallInputWnd = new CBallInput;
	m_pBallInputWnd->CreateWnd(this,BALL_INPUTWND_LEFT,BALL_INPUTWND_TOP);

	
	// 공의 힘을 입력 받는 콘트롤
	m_sliderCtrl.Create(TBS_HORZ,
		CRect(SLIDER_LEFT,SLIDER_TOP,SLIDER_LEFT+SLIDER_WIDTH,SLIDER_TOP+SLIDER_HEIGHT),
		this,IDC_SLIDER_POWER);
	m_sliderCtrl.ShowWindow(SW_SHOW);	  
	m_sliderCtrl.SetRangeMax(MAX_BALL_V0_SPEED);

	// 공을 발사하는 콘트롤 
	m_buttonFire.Create("Fire",WS_CHILD|WS_VISIBLE,
		CRect(FBUTTON_LEFT,FBUTTON_TOP,FBUTTON_LEFT+FBUTTON_WIDTH,FBUTTON_TOP+FBUTTON_HEIGHT),
		this,IDC_BUTTON_FIRE);

	// 메시지 출력용 콘트롤.
	m_EditCtrl.Create(
		ES_AUTOHSCROLL|
		ES_MULTILINE|
		ES_WANTRETURN|
		//WS_EX_RIGHTSCROLLBAR|
		WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_VSCROLL,//ES_READONLY|
		CRect(RE_LEFT,RE_TOP,RE_LEFT+RE_WIDTH,RE_TOP+RE_HEIGHT),
		this,IDC_RICH_EDIT);


	// 전역 변수에 포인터 주기.
	pEditCtrl = &m_EditCtrl;

	// 채팅용 콘트롤.
	m_EditChat.Create(
		//ES_AUTOVSCROLL|
		//ES_MULTILINE|
		//ES_WANTRETURN|
		//WS_EX_RIGHTSCROLLBAR|
		WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,//ES_READONLY|
		CRect(CHAT_EDIT_LEFT,CHAT_EDIT_TOP,CHAT_EDIT_LEFT+CHAT_EDIT_WIDTH,CHAT_EDIT_TOP+CHAT_EDIT_HEIGHT),
		this,IDC_CHAT_EDIT);

}

// 공을 발사한다.
void CJDRView::OnButtonFire()
{

	CBoardView*	pBoardView;
	pBoardView = ((CJDRDoc*)GetDocument())->m_pBoardView;

	//m_nBallNum = pBoardView->m_board.m_nCurBallNum;
	m_dPower = m_sliderCtrl.GetPos();
	m_pBallInputWnd->GetXY(m_dBx,m_dBy);

	if(!pBoardView->m_bMovingNow)				// 공이 멈추었을때.
		pBoardView->Stroke(
			m_nBallNum,
			m_dBx,m_dBy,
			m_dPower);
	else 
		debug("에러-공이 움직이고 있는데 Stroke()수행..");
}

void CJDRView::OnDestroy() 
{
	CView::OnDestroy();
}


// 서버 상태로 대기한다.
void CJDRView::OnNethost() 
{
}


// 나는 클라이언트, 서버에 연결시도.
void CJDRView::OnNetconnect() 
{
}

// 연결 종료 시도.
void CJDRView::OnNetdisconnect() 
{
}


int CJDRView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	return 0;
}

void CJDRView::OnUpdateNetconnect(CCmdUI* pCmdUI) 
{
	
}

void CJDRView::OnUpdateNetdisconnect(CCmdUI* pCmdUI) 
{
}

void CJDRView::OnUpdateNethost(CCmdUI* pCmdUI) 
{
}


BOOL CJDRView::PreTranslateMessage(MSG* pMsg) 
{
	// 에디트 박스에서 엔터키를 누를때 
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (GetDlgItem(IDC_CHAT_EDIT)->m_hWnd == pMsg->hwnd)
		{
			//pMsg->wParam=VK_TAB;
			TransChatData();
		}
		else 
		{
			//pMsg->wParam=VK_TAB;
		}
	}
	return CView::PreTranslateMessage(pMsg);
}

void CJDRView::TransChatData()
{
}

// CBoardView에서 네트웍으로 전송할때 호출한다.
void CJDRView::SendStrokeDataToNetwork(const StrokeStatus &ss)
{
}

// m_pSock 에서 각종 데이타를 받았을 경우 SendMessage로 여기를 호출한다.
LONG CJDRView::OnReceiveData(UINT, LONG)
{
	return TRUE;
}

void CJDRView::OnSetPlayer1() 
{
	m_nBallNum = 0;
	msg("당신은 쩜공입니다.");
}

void CJDRView::OnSetPlayer2() 
{
	m_nBallNum = 1;
	msg("당신은 안쩜공입니다.");
}

void CJDRView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	static bool bFirst = true;
	if(bFirst) {
		// 배경 DC생성
		m_dcMEMBackGnd.CreateCompatibleDC(&dc);
		m_bmBackGnd.LoadBitmap(IDB_BACKGND);     // bitmap 읽어 들이기.
		m_pOldBitmapBackGnd = m_dcMEMBackGnd.SelectObject(&m_bmBackGnd);
		bFirst = false;
	}


	dc.BitBlt(0,0,JDR_VIEW_WIDTH,JDR_VIEW_HEIGHT,
		&m_dcMEMBackGnd,0,0,SRCCOPY);
}

BOOL CJDRView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
