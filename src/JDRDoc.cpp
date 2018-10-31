// JDRDoc.cpp : implementation of the CJDRDoc class
//

#include "stdafx.h"
#include "JDR.h"
#include "Util.h"
#include "JDRDoc.h"
#include "Playsnd.h"
#include "JDRView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJDRDoc

IMPLEMENT_DYNCREATE(CJDRDoc, CDocument)

BEGIN_MESSAGE_MAP(CJDRDoc, CDocument)
	//{{AFX_MSG_MAP(CJDRDoc)
	ON_COMMAND(IDM_TEST, OnTest)
	ON_COMMAND(IDM_TEST2, OnTest2)
	ON_COMMAND(IDM_READ_CONSTANT, OnReadConstant)
	ON_COMMAND(IDM_MEMORY_BALL_POS, OnMemoryBallPos)
	ON_COMMAND(IDM_REMEMBER_BALL_POS, OnRememberBallPos)
	ON_COMMAND(IDM_AI_30, OnAi30)
	ON_COMMAND(IDM_AI_50, OnAi50)
	ON_COMMAND(IDM_SPEED_TEST, OnSpeedTest)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(IDM_AI_BPN, OnAiBpn)
	ON_COMMAND(IDM_AI_80, OnAi80)
	ON_COMMAND(IDM_AI_100, OnAi100)
	ON_COMMAND(IDM_AI_150, OnAi150)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJDRDoc construction/destruction

CJDRDoc::CJDRDoc()
{
	m_pBoardView  = NULL;
}

CJDRDoc::~CJDRDoc()
{
	if(m_pBoardView) delete m_pBoardView;
}

BOOL CJDRDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	/* BoradView 생성 */

	POSITION pos = GetFirstViewPosition();
	m_pView = (CJDRView*) GetNextView(pos);

	m_pBoardView = new CBoardView();
	// CJDRView 의 센터에 위치 
	m_pBoardView->CreateWin((CWnd*)m_pView, BOARD_VIEW_TOP, (JDR_VIEW_WIDTH-BOARD_VIEW_WIDTH)/2);
	m_pBoardView->SetNewGame(250,100);



	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJDRDoc serialization

void CJDRDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJDRDoc diagnostics

#ifdef _DEBUG
void CJDRDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJDRDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJDRDoc commands

void CJDRDoc::OnTest() 
{
	m_pBoardView->Test();	
}

void CJDRDoc::OnTest2() 
{
	m_pBoardView->Test2();	
	
}

void CJDRDoc::OnReadConstant() 
{
	// 공이 상수 읽어 들이기 
	m_pBoardView->m_board.GetBall(0)->ReadConstant();
	
}

void CJDRDoc::OnMemoryBallPos() 
{
	m_pBoardView->MemoryBallPos();
}

void CJDRDoc::OnRememberBallPos() 
{
	m_pBoardView->RememberBallPos();	
}


void CJDRDoc::OnAi30() 
{
	m_pBoardView->DoAI30();	
}

void CJDRDoc::OnAi50() 
{
	m_pBoardView->DoAI50();	
}
void CJDRDoc::OnAi80() 
{
	m_pBoardView->DoAI80();	
}

void CJDRDoc::OnAi100() 
{
	m_pBoardView->DoAI100();	
}
void CJDRDoc::OnAi150() 
{
	m_pBoardView->DoAI150();	
}
void CJDRDoc::OnAiBpn() 
{
	m_pBoardView->DoAIBpn();	
}

void CJDRDoc::OnSpeedTest() 
{
	m_pBoardView->SpeedTest();	
}

void CJDRDoc::OnFileOpen() 
{
	// TODO: Add your command handler code here
	
}

#define FILE_NAME	"BPN-DATA.txt"

void CJDRDoc::OnFileSave() 
{

	m_pBoardView->DisableMMTimer();

	FILE* fp;
	int   nBallNum = 0;						// 사람.


	fp = fopen(FILE_NAME,"a+");
	if(fp==NULL) {
		AfxMessageBox("파일 저장 실패");
		return;
	}
	int i;
	// 여기는 입력 에 관계 되는 부분.
	fprintf(fp,"%d\t",nBallNum);
	for(i=0;i<BALL_NUM;i++) {
		fprintf(fp,"%f\t%f\t",
			m_pBoardView->m_board.GetBall(i)->GetX(),
			m_pBoardView->m_board.GetBall(i)->GetY()
			);
	}
	// 여기는 출력에 관계되는 부분.
/*
	fprintf(fp,"%f\t%f\t%f\t%f\t%f\n",
		m_pBoardView->GetDX(nBallNum),				
		m_pBoardView->GetDY(nBallNum),
		m_pBoardView->m_pDlg->GetBX(),
		m_pBoardView->m_pDlg->GetBY(),
		m_pBoardView->m_pDlg->GetPower()
		);
*/
	fprintf(fp,"%f\t%f\t%f\t%f\t%f\n",
		m_pBoardView->GetDX(nBallNum),				
		m_pBoardView->GetDY(nBallNum),
		m_pView->m_pBallInputWnd->GetBX(),
		m_pView->m_pBallInputWnd->GetBY(),
		m_pView->m_dPower
		);

	fclose(fp);

	m_pBoardView->SetMMTimer();

}

void CJDRDoc::OnFileSaveAs() 
{
}




