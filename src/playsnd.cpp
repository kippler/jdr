

#include "stdafx.h"
#include "mmsystem.h"
#include "playsnd.h"
extern HINSTANCE ghInstance;

BOOL PlayResource(char* szResName)  
{ 
//return TRUE;
    BOOL bRtn; 
    LPSTR lpRes; 
    HANDLE  hRes; 
	HRSRC hResInfo;
 
    /* Find the WAVE resource. */ 
    hResInfo = FindResource(AfxGetInstanceHandle(),szResName, "WAVE"); 
    if (hResInfo == NULL) 
        return FALSE; 
    /* Load the WAVE resource. */ 
    hRes = LoadResource(AfxGetInstanceHandle(), hResInfo); 
    if (hRes == NULL) 
        return FALSE; 
 
    /* Lock the WAVE resource and play it. */ 
 
    lpRes = (char*)LockResource(hRes); 
    if (lpRes != NULL) { 
//        bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | 
//            SND_NODEFAULT); 
        bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_ASYNC | //SND_NOSTOP |
            SND_NODEFAULT); 
        UnlockResource(hRes); 
    } 
    else 
        bRtn = 0; 
 
    /* Free the WAVE resource and return success or failure. */ 
 
    FreeResource(hRes); 
    return bRtn; 
} 
 
