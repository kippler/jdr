///////////////////////////////////////////////////////////
// CDIB.CPP: Implementation file for the DIB class.
///////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cdib.h"
#include "windowsx.h"

#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

///////////////////////////////////////////////////////////
// CDib::CDib()
///////////////////////////////////////////////////////////
CDib::CDib(void)
{
	m_hBitmap=NULL;	
	m_pBmInfo = (LPBITMAPINFO) malloc (sizeof(BITMAPINFO));
}
///////////////////////////////////////////////////////////
// CDib::~CDib()
///////////////////////////////////////////////////////////
CDib::~CDib()
{
   if(m_pBmInfo) free(m_pBmInfo);
   if(m_hBitmap) ::DeleteObject(m_hBitmap);
   m_pBmInfo=NULL;
   m_hBitmap=NULL;
   // Free the memory assigned to the bitmap.
}

///////////////////////////////////////////////////////////
// CDib::LoadBitmapFile()
//
// This function loads a DIB from disk into memory. It also
// initializes the various class data members.
///////////////////////////////////////////////////////////
HBITMAP CDib::LoadBitmapFile(HDC hdc, const char* fileName)
{		
	if(m_hBitmap){
		::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}						
	
	CFile file;
	file.Open(fileName,CFile::modeRead);

    // Read the bitmap's file header into memory.
    BITMAPFILEHEADER bmFileHeader;
	//fread((void*)&bmFileHeader,1,sizeof(bmFileHeader),fp);
	file.Read((void*)&bmFileHeader,sizeof(bmFileHeader));

    // Check whether the file is really a bitmap.
    if (bmFileHeader.bfType != 0x4d42)
    {
        AfxMessageBox("Not a bitmap file");
    }    
    else
    {		
		file.Read((void*)m_pBmInfo,sizeof(BITMAPINFOHEADER));
		//fread((void*)m_pBmInfo,1, sizeof(BITMAPINFOHEADER),fp);
		
		LONG  dibSize = WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount)*m_pBmInfo->bmiHeader.biHeight;
		m_hBitmap=CreateDIBSection(hdc,m_pBmInfo,DIB_RGB_COLORS,(void **) &m_pBits, NULL, 0);
		ASSERT(m_pBits);	 			   
        // Read the bitmap into memory and close the file.
		file.Read((void*)m_pBits,dibSize);
		//fread((void*)m_pBits,1,dibSize,fp);
		file.Close();
		//fclose(fp);
		return m_hBitmap;
    }
	
	return NULL;
}

///////////////////////////////////////////////////////////
// CDib::GetDibSizeImage()
//
// This function calculates and returns the size of the
// bitmap's image in bytes.
///////////////////////////////////////////////////////////
DWORD CDib::GetDibSizeImage()
{
    // If the bitmap's biSizeImage field contains
    // invalid information, calculate the correct size.
    if (m_pBmInfo->bmiHeader.biSizeImage == 0)
    {
        // Get the width in bytes of a single row.
        DWORD byteWidth = (DWORD) GetDibWidth();

        // Get the height of the bitmap.
        DWORD height = (DWORD) GetDibHeight();

        // Multiply the byte width by the number of rows.
        DWORD imageSize = byteWidth * height;

        return imageSize;
    }
    // Otherwise, just return the size stored in
    // the BITMAPINFOHEADER structure.
    else
        return m_pBmInfo->bmiHeader.biSizeImage;
}

///////////////////////////////////////////////////////////
// CDib::GetDibWidth()
//
// This function returns the width in bytes of a single
// row in the bitmap.
///////////////////////////////////////////////////////////
UINT CDib::GetDibWidth()
{
	if(m_hBitmap==NULL) return 0;
    return (UINT) m_pBmInfo->bmiHeader.biWidth;
}

///////////////////////////////////////////////////////////
// CDib::GetDibHeight()
//
// This function returns the bitmap's height in pixels.
///////////////////////////////////////////////////////////
UINT CDib::GetDibHeight()
{
	if(m_hBitmap==NULL) return 0;
    return (UINT) m_pBmInfo->bmiHeader.biHeight;
}

///////////////////////////////////////////////////////////
// CDib::GetDibNumColors()
//
// This function returns the number of colors in the bitmap.
///////////////////////////////////////////////////////////
UINT CDib::GetDibNumColors()
{
    if ((m_pBmInfo->bmiHeader.biClrUsed == 0) &&
          (m_pBmInfo->bmiHeader.biBitCount < 9))
        return (1 << m_pBmInfo->bmiHeader.biBitCount);
    else
        return (int) m_pBmInfo->bmiHeader.biClrUsed;
}

///////////////////////////////////////////////////////////
// CDib::GetDibInfoHeaderPtr()
//
// This function returns a pointer to the bitmap's
// BITMAPINFOHEADER structure.
///////////////////////////////////////////////////////////
LPBITMAPINFOHEADER CDib::GetDibInfoHeaderPtr()
{
    return &(m_pBmInfo->bmiHeader);
}

///////////////////////////////////////////////////////////
// CDib::GetDibInfoPtr()
//
// This function returns a pointer to the bitmap's
// BITMAPINFO structure.
///////////////////////////////////////////////////////////
LPBITMAPINFO CDib::GetDibInfoPtr()
{
    return m_pBmInfo;
}


///////////////////////////////////////////////////////////
// CDib::GetDibBitsPtr()
//
// This function returns a pointer to the bitmap's
// actual image data.
///////////////////////////////////////////////////////////
BYTE* CDib::GetDibBitsPtr()
{
    return m_pBits;
}
BOOL CDib::CopyDIBtoDIB(CDib *pDibSrc,int x,int y,int nStyle,BYTE r,BYTE g,BYTE b)
{
	int sx=0;
	int sy=0;
	int dx=GetDibWidth();
	int dy=GetDibHeight();
	return CopyDIBtoDIB(pDibSrc,sx,sy,dx,dy,x,y,nStyle,r,g,b);
}
BOOL CDib::CopyDIBtoDIB(CDib *pDibSrc, int sx, int sy, int dx, int dy, int x, int y, int nStyle, BYTE r,BYTE g,BYTE b)
{	
	int WidthCnt=0;

	if(sx>dx) return FALSE;
	if(sy>dy) return FALSE;

	if(y+(dy-sy)<=0) 
		return TRUE;
	if(y<0) {
		sy-=y;
		y=0;
	}


	if(dx>pDibSrc->m_pBmInfo->bmiHeader.biWidth) dx=pDibSrc->m_pBmInfo->bmiHeader.biWidth;
	if(dy>pDibSrc->m_pBmInfo->bmiHeader.biHeight) dy=pDibSrc->m_pBmInfo->bmiHeader.biHeight;

	int nSinc;
	int nDinc;

	int nWidth=dx-sx;
	int nHeight=dy-sy;
	int nScanS=WIDTHBYTES((DWORD)pDibSrc->m_pBmInfo->bmiHeader.biWidth * pDibSrc->m_pBmInfo->bmiHeader.biBitCount);
	int nScanD=WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount);

	BYTE *pDest = (BYTE*)GetPixelAddress(x,y+nHeight-1);
	ASSERT(pDest);
	BYTE *pSrc = (BYTE*)GetPixelAddress(sx,sy+nHeight-1,pDibSrc);
	BYTE Alpha;
	ASSERT(pSrc);

	switch(nStyle){
	case DIB_COPY:
		while(nHeight--){
			memcpy(pDest,pSrc,nWidth*(m_pBmInfo->bmiHeader.biBitCount/8));
			pSrc+=nScanS;
			pDest+=nScanD;
		}
		break;
	case DIB_TRANSPARENT:
		nSinc=nScanS-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;
		nDinc=nScanD-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;

		while(nHeight--){
			WidthCnt=nWidth;
			while(WidthCnt--){
				// Adobe PhotoShop 은 PSD작업을 BMP로 옮기면, 
				// 255,0,255가 255,3,255가 된다. 그래서 G값이 3인 것도 투명색 
				// 처리한다.
				if(*(pSrc)==r && (*(pSrc+1)==g||*(pSrc+1)==3)&& *(pSrc+2)==b){
					pDest+=(m_pBmInfo->bmiHeader.biBitCount/8);
				}
				else{
					memcpy(pDest,pSrc,m_pBmInfo->bmiHeader.biBitCount/8);
					pDest+=(m_pBmInfo->bmiHeader.biBitCount/8);
				}
				pSrc+=3;
			}
			pSrc +=nSinc;
			pDest+=nDinc;
		}
		break;
	case DIB_ALPHA:
		nSinc=nScanS-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;
		nDinc=nScanD-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;

		while(nHeight--){
			WidthCnt=nWidth;
			while(WidthCnt--){
				for(int i=0;i<m_pBmInfo->bmiHeader.biBitCount/8;i++){
					if(*(pSrc)!=255 && *(pSrc+1)!=255 && *(pSrc+2)!=255){
						Alpha = (BYTE)(((UINT)(*pDest)+(UINT)(*pSrc))>>1);
						*pDest=Alpha;
					}
					pDest++;
					pSrc++;
				}
			}
			pSrc +=nSinc;
			pDest+=nDinc;
		}
		break;
	case DIB_ALPHA2:
		nSinc=nScanS-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;
		nDinc=nScanD-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;

		while(nHeight--){
			WidthCnt=nWidth;
			while(WidthCnt--){
				for(int i=0;i<m_pBmInfo->bmiHeader.biBitCount/8;i++){					
					Alpha = (BYTE)(((UINT)(*pDest)*20+(UINT)(*pSrc)*80)/100);					
					*pDest=Alpha;					
					pDest++;
					pSrc++;
				}
			}
			pSrc +=nSinc;
			pDest+=nDinc;
		}
		break;
	case DIB_BLUR:
		nSinc=nScanS-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;
		nDinc=nScanD-nWidth*m_pBmInfo->bmiHeader.biBitCount/8;

		while(nHeight--){
			WidthCnt=nWidth;
			while(WidthCnt--){
				if(*(pSrc)==r && *(pSrc+1)==g && *(pSrc+2)==b){
					pDest+=(m_pBmInfo->bmiHeader.biBitCount/8);
					pSrc+=(m_pBmInfo->bmiHeader.biBitCount/8);
				}
				else{
					for(int i=0;i<m_pBmInfo->bmiHeader.biBitCount/8;i++){
						if((*pSrc)<(*pDest)){
							Alpha = (BYTE)(((UINT)(*pDest)*3+(UINT)(*pSrc)*7)/10);
							*pDest=Alpha;
						}
						pDest++;
						pSrc++;
					}
				}					
			}
			pSrc +=nSinc;
			pDest+=nDinc;
		}
		break;	
	}			

	return TRUE;
}

HBITMAP CDib::CreateNullImage(HDC hdc,int nWidth, int nHeight, int BitCount)
{		
	if(m_hBitmap){
		::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}
	m_pBmInfo->bmiHeader.biSize=0x28;
	m_pBmInfo->bmiHeader.biWidth=nWidth;
	m_pBmInfo->bmiHeader.biHeight=nHeight;
	m_pBmInfo->bmiHeader.biPlanes=1;
	m_pBmInfo->bmiHeader.biBitCount=BitCount;
	m_pBmInfo->bmiHeader.biCompression=0;
	m_pBmInfo->bmiHeader.biSizeImage=WIDTHBYTES((DWORD)nWidth * BitCount) * nHeight;
	m_pBmInfo->bmiHeader.biXPelsPerMeter=0xece;
	m_pBmInfo->bmiHeader.biYPelsPerMeter=0xec4;
	m_pBmInfo->bmiHeader.biClrUsed=0;
	m_pBmInfo->bmiHeader.biClrImportant=0;

	m_hBitmap=CreateDIBSection(hdc,m_pBmInfo,DIB_RGB_COLORS,(void **) &m_pBits, NULL, 0);

	ASSERT(m_hBitmap);

	return m_hBitmap;
}

void* CDib::GetPixelAddress(int x, int y,CDib *psrc)
{
	if(psrc==NULL){
		int ByteCount= (m_pBmInfo->bmiHeader.biBitCount/8);
		int iWidth  = m_pBmInfo->bmiHeader.biWidth;	
		return m_pBits+(GetDibHeight()-y-1)*
			WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount)+ x*ByteCount;
	}
	else {
		int ByteCount= (psrc->m_pBmInfo->bmiHeader.biBitCount/8);		
		return psrc->GetDibBitsPtr()+(psrc->GetDibHeight()-y-1)*
			WIDTHBYTES((DWORD)psrc->m_pBmInfo->bmiHeader.biWidth * psrc->m_pBmInfo->bmiHeader.biBitCount)+ x*ByteCount;
	}
}

HBITMAP CDib::GetHandleBitmap()
{
	return m_hBitmap;
}

HBITMAP CDib::LoadBitmap(HDC hdc, UINT nIDResource)
{
	if(m_hBitmap){
		::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}

	HRSRC hRsrcImage = ::FindResource(AfxGetInstanceHandle(),(LPCTSTR)MAKEINTRESOURCE(nIDResource),RT_BITMAP);
	if (hRsrcImage==NULL) return NULL;
	HGLOBAL hMem=LoadResource(AfxGetInstanceHandle(),hRsrcImage);
	BYTE *mem =(BYTE*) LockResource(hMem);
	int temp=GetLastError();
	memcpy(m_pBmInfo,mem,sizeof(BITMAPINFOHEADER));
	LONG  dibSize = WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount)*m_pBmInfo->bmiHeader.biHeight;
	m_hBitmap=CreateDIBSection(hdc,m_pBmInfo,DIB_RGB_COLORS,(void **) &m_pBits, NULL, 0);
	ASSERT(m_pBits);	 		
	mem+=sizeof(BITMAPINFOHEADER);
	memcpy(m_pBits,mem,dibSize);
	FreeResource(hMem);
	return m_hBitmap;
}

void CDib::FillGrayColor(int sx, int sy, int dx, int dy, BYTE key)
{

	if(sx>dx) return;
	if(sy>dy) return;

	if(dx>m_pBmInfo->bmiHeader.biWidth) dx=m_pBmInfo->bmiHeader.biWidth;
	if(dy>m_pBmInfo->bmiHeader.biHeight-1) dy=m_pBmInfo->bmiHeader.biHeight-1;

	int nWidth=dx-sx;
	int nHeight=dy-sy;
	int nScanD=WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount);

	BYTE *pDest = (BYTE*)GetPixelAddress(sx,sy+nHeight-1);
	ASSERT(pDest);

	while(nHeight--){
		memset(pDest,(int)key,nWidth*(m_pBmInfo->bmiHeader.biBitCount/8));
		pDest+=nScanD;
	}
}

void CDib::PutPixel(int x,int y,BYTE r,BYTE g,BYTE b)
{
	BYTE *pDest = (BYTE*)GetPixelAddress(x,y);
	if(x>(int)GetDibWidth()-1) return;
	if(y>(int)GetDibHeight()-1) return;
	*pDest= b;
	*(pDest+1) = g;
	*(pDest+2) = r;
}

void CDib::DeleteDIB()
{
   if(m_hBitmap) ::DeleteObject(m_hBitmap);
   m_hBitmap=NULL;
}

COLORREF  CDib::GetPixelColor(int x, int y)
{	
	COLORREF rgb;

	BYTE *pSrc = (BYTE*)GetPixelAddress(x,y);

	rgb =RGB(*(pSrc+2),*(pSrc+1),*pSrc);

	return rgb;
}

void CDib::PutPixel(int x, int y, COLORREF rgb)
{
	BYTE *pDest = (BYTE*)GetPixelAddress(x,y);
	if(x>(int)GetDibWidth()-1) return;
	if(y>(int)GetDibHeight()-1) return;
	if(x<0) return;
	if(y<0) return;
	*pDest=GetBValue(rgb);
	*(pDest+1)=GetGValue(rgb);
	*(pDest+2)=GetRValue(rgb);
}

void CDib::FillColor(BYTE r, BYTE g, BYTE b)
{
	BYTE *pDest;
	BYTE *pSrc = (BYTE*)GetPixelAddress(0,0);
	for(UINT i=0;i<GetDibWidth();i++){
		*pSrc=b;
		*(pSrc+1)=g;
		*(pSrc+2)=r;
		pSrc+=3;
	}	
	pSrc=(BYTE*)GetPixelAddress(0,0);
	for(UINT j=1;j<GetDibHeight();j++){
		pDest=(BYTE*)GetPixelAddress(0,j);
		memcpy(pDest,pSrc,(GetDibWidth())*3);
	}
}

void CDib::FillColor(COLORREF rgb)
{
	BYTE *pDest;
	BYTE *pSrc = (BYTE*)GetPixelAddress(0,0);
	for(UINT i=0;i<GetDibWidth();i++){
		*pSrc=GetBValue(rgb);
		*(pSrc+1)=GetGValue(rgb);
		*(pSrc+2)=GetRValue(rgb);
		pSrc+=3;
	}	
	pSrc=(BYTE*)GetPixelAddress(0,0);
	for(UINT j=1;j<GetDibHeight();j++){
		pDest=(BYTE*)GetPixelAddress(0,j);
		memcpy(pDest,pSrc,(GetDibWidth())*3);
	}

}

void CDib::FillColorRect(int x,int y,int R, BYTE r, BYTE g, BYTE b)
{
	int h,w;

	BYTE *pDest;

	w=0;
	for(h=0;h<R;h++){
		pDest=(BYTE*)GetPixelAddress(x-h*2,y-(R-h));
		for(int i=0;i<w;i++){
			*pDest=b;
			*(pDest+1)=g;
			*(pDest+2)=r;
			pDest+=3;
		}
		w+=2;
	}
	for(h=0;h<R;h++){
		pDest=(BYTE*)GetPixelAddress(x-(R-h)*2,y+h);
		for(int i=0;i<w;i++){
			*pDest=b;
			*(pDest+1)=g;
			*(pDest+2)=r;
			pDest+=3;
		}
		w-=2;
	}
}

void CDib::FillColorRect(int x, int y, int R, COLORREF rgb)
{
	R=15;
	// 여기서는 강제적으로 원을 지름 15짜리로 마스크를 만들어서 
	// 원을 그렸다... 속도문제도 있고.. 또 딴데도 쓸데 없고 해서이다...
	// 조만간에 수정을 할 것이다.

	int MaskTable[15][15]={
		{0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
		{0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
	};

	BYTE *pDest;

	for(int l_y=0;l_y<15;l_y++){
		for(int l_x=0;l_x<15;l_x++){
			if(MaskTable[l_y][l_x]==0) continue;
			pDest=(BYTE*)GetPixelAddress(x+l_x,y+l_y);
			*pDest=GetBValue(rgb);
			*(pDest+1)=GetGValue(rgb);
			*(pDest+2)=GetRValue(rgb);
		}
	}
}

void CDib::AddPixel(int x, int y, BYTE r, BYTE g, BYTE b)
{
	BYTE *pDest = (BYTE*)GetPixelAddress(x,y);
	if(x>(int)GetDibWidth()-1) return;
	if(y>(int)GetDibHeight()-1) return;
/*	if(*pDest<r)			*pDest		 =0;
	else					*pDest		-=r;
	if(*(pDest+1)<g)		*(pDest+1)	 =0;
	else					*(pDest+1)	-=g;
	if(*(pDest+2)<b)		*(pDest+2)	 =0;
	else					*(pDest+2)	-=b;
*/
	*pDest =	 *pDest     & b;
	*(pDest+1) = *(pDest+1) & g;
	*(pDest+2) = *(pDest+2) & r;
}

void CDib::FillColorUseMask(int x, int y, int MaskTable[15][15],COLORREF rgb)
{
	BYTE *pDest;

	for(int l_y=0;l_y<15;l_y++){
		for(int l_x=0;l_x<15;l_x++){
			if(MaskTable[l_y][l_x]==0) continue;
			pDest=(BYTE*)GetPixelAddress(x+l_x,y+l_y);
			*pDest=GetBValue(rgb);
			*(pDest+1)=GetGValue(rgb);
			*(pDest+2)=GetRValue(rgb);
		}
	}
}

void CDib::StretchCopyDIBtoDIB(CDib * pDib,int nWidth,int nHeight,int p_x,int p_y)
{
	FillColor(255,255,0);
}
void CDib::StretchCopyDIBtoDIB(CDib * pDib,int nFactor)
{	
}

BOOL CDib::CopyDIBtoDIB2(CDib * pDibSrc, int tx, int ty, int dx, int dy, int orig, int border, int alpha)
{
	int x,y;

	int nSinc;
	int nDinc;

	int nScanS=WIDTHBYTES((DWORD)pDibSrc->m_pBmInfo->bmiHeader.biWidth * pDibSrc->m_pBmInfo->bmiHeader.biBitCount);
	int nScanD=WIDTHBYTES((DWORD)m_pBmInfo->bmiHeader.biWidth * m_pBmInfo->bmiHeader.biBitCount);

	nSinc=nScanS-dx*m_pBmInfo->bmiHeader.biBitCount/8;
	nDinc=nScanD-dx*m_pBmInfo->bmiHeader.biBitCount/8;

	BYTE *pDest = (BYTE*)GetPixelAddress(tx,ty+dy-1);
	ASSERT(pDest);
	BYTE *pOrig = (BYTE*)GetPixelAddress(orig*dx,dy-1,pDibSrc);
	ASSERT(pOrig);
	BYTE *pBorder = (BYTE*)GetPixelAddress(border*dx,dy-1,pDibSrc);
	ASSERT(pBorder);
	BYTE *pAlpha = (BYTE*)GetPixelAddress(alpha*dx,dy-1,pDibSrc);
	ASSERT(pAlpha);

	for ( y = 0 ; y < dy ; y++ )
	{
		for ( x = 0 ; x < dx ; x++ )
		{
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
		}
		pDest	+= nDinc;
		pOrig	+= nSinc;
		pBorder += nSinc;
		pAlpha	+= nSinc;
	}

	return TRUE;
}

void CDib::CopyDIBtoDIB(CDib * pDibSrc, CDib * pDibAlpha)
{
	UINT x,y;

	ASSERT(GetDibHeight()&pDibSrc->GetDibHeight()&pDibAlpha->GetDibHeight()&
		GetDibWidth()&pDibSrc->GetDibWidth()&pDibAlpha->GetDibWidth());

	BYTE *pDest = (BYTE*)GetPixelAddress(0,GetDibHeight()-1);
	ASSERT(pDest);
	BYTE *pOrig = (BYTE*)GetPixelAddress(0,GetDibHeight()-1);
	ASSERT(pOrig);
	BYTE *pBorder = (BYTE*)(pDibSrc->GetPixelAddress(0,pDibSrc->GetDibHeight()-1));
	ASSERT(pBorder);
	BYTE *pAlpha  = (BYTE*)(pDibAlpha->GetPixelAddress(0,pDibAlpha->GetDibHeight()-1));
	ASSERT(pAlpha);

	for ( y = 0 ; y < GetDibHeight(); y++ )
	{
		for ( x = 0 ; x < GetDibWidth(); x++ )
		{
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
			*pDest++ = (((int)*pOrig++) * (255 - *pAlpha) + ((int)*pBorder++)* *pAlpha)/255;	pAlpha++;
		}
	}
}

void CDib::SetBmInfo(BITMAPINFO BmInfo)
{
	m_pBmInfo->bmiHeader.biSize=BmInfo.bmiHeader.biSize;
	m_pBmInfo->bmiHeader.biWidth=BmInfo.bmiHeader.biWidth;
	m_pBmInfo->bmiHeader.biHeight=BmInfo.bmiHeader.biHeight;
	m_pBmInfo->bmiHeader.biPlanes=BmInfo.bmiHeader.biPlanes;
	m_pBmInfo->bmiHeader.biBitCount=BmInfo.bmiHeader.biBitCount;
	m_pBmInfo->bmiHeader.biCompression=BmInfo.bmiHeader.biCompression;
	m_pBmInfo->bmiHeader.biSizeImage=BmInfo.bmiHeader.biSizeImage;
	m_pBmInfo->bmiHeader.biXPelsPerMeter=BmInfo.bmiHeader.biXPelsPerMeter;
	m_pBmInfo->bmiHeader.biYPelsPerMeter=BmInfo.bmiHeader.biYPelsPerMeter;
	m_pBmInfo->bmiHeader.biClrUsed=BmInfo.bmiHeader.biClrUsed;
	m_pBmInfo->bmiHeader.biClrImportant=BmInfo.bmiHeader.biClrImportant;
}
