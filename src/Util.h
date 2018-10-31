//////////////////////////////////////////////////////////
//  UTIL.H  --  전역 함수 정의 
//////////////////////////////////////////////////////////

#ifndef _UTIL_H_
#define _UTIL_H_

//#include "stdafx.h"

#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616


void _cdecl debug(const char* str, ...);
void debug_end();

void TimerOn();
void Timer(char* str);


extern int		TickCount;
class CSWatch {
private :
	DWORD	m_time;
public :
	CSWatch(){Set(0);};
	void Set(int time){m_time=TickCount+time;}
	bool OK(){return (int(TickCount-m_time)<0)?false:true;}
};



void CheckMsg();

CString  GetThisPath();

#endif // _UTIL_H_