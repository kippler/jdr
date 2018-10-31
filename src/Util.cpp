
#include "stdafx.h"
#include "util.h"

int		TickCount = 0;

HANDLE	m_hMapObject = NULL;
LPLONG	m_pMapView;
		  	  
void _cdecl debug(const char* str, ...)
{
	/*
	static bool		bFirst = true;
	if(bFirst) {
		bFirst = false;
		m_hMapObject = CreateFileMapping((HANDLE)-1,NULL,
			PAGE_READWRITE,0,sizeof(DWORD),"shared_memory");
		m_pMapView = (LPLONG)::MapViewOfFile(
			m_hMapObject,FILE_MAP_WRITE,0,0,0);
		strcpy((char*)m_pMapView,"DEBUG STARTS HERE......");
	}

	va_list args;
	va_start(args, str);

	int nBuf;
	TCHAR szBuffer[512];
	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer), str, args);

	// was there an error? was the expanded string too long?
	ASSERT(nBuf >= 0);

	if(m_pMapView[0]==-1 || strlen((char*)m_pMapView)>5000 )
		strcpy((char*)m_pMapView,szBuffer);
	else {
		strcat((char*)m_pMapView,"\n");
		strcat((char*)m_pMapView,szBuffer);
	}

	va_end(args);
	*/
}


void debug_end()
{
	if(m_hMapObject)
		CloseHandle(m_hMapObject);
}

int nStartTime;
void TimerOn()
{
	nStartTime = GetTickCount();
	debug("TIMER ON");
}
void Timer(char* str)
{
	CString szBuf;
	szBuf.Format("%s : %d",str,(GetTickCount()-nStartTime));
	debug((const char*)szBuf);
}


// 잠깐 짬을 내서 메시지를 처리해 준다.
void CheckMsg()
{

	CWinThread	*pT;
//	MSG*	pMsg;
//	pMsg= &(t->m_msgCur);
	pT = (CWinThread*)AfxGetApp();

	pT->PumpMessage();
/*

	// pump message, but quit on WM_QUIT
	if (!PumpMessage())
		return ExitInstance();

	// reset "no idle" state after pumping "normal" message
	if (IsIdleMessage(&m_msgCur))
	{
		bIdle = TRUE;
		lIdleCount = 0;
	}

	} while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE));
*/
}

CString  GetThisPath()
{
	static TCHAR szModuleName[MAX_PATH*4];
	static BOOL bIsFirst = TRUE;
	if(bIsFirst)
	{
		::GetModuleFileName(NULL,szModuleName,MAX_PATH*4);
		*(_tcsrchr( szModuleName, '\\' ) + 1) = NULL;
		bIsFirst = FALSE;
	}
	return CString(szModuleName);
}
