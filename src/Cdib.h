///////////////////////////////////////////////////////////
// CDIB.H: Header file for the DIB class.
///////////////////////////////////////////////////////////

#ifndef __CDIB_H
#define __CDIB_H

class CDib : public CObject
{
public:
	HBITMAP m_hBitmap;

protected:
    LPBITMAPFILEHEADER m_pBmFileHeader;
    LPBITMAPINFO m_pBmInfo;
    RGBQUAD* m_pRGBTable;
    BYTE* m_pDibBits;
    UINT m_numColors;
	BOOL m_bGlobalAlloc;
	BYTE *m_pBits;

public:    
	void SetBmInfo(BITMAPINFO BmInfo);
	void CopyDIBtoDIB(CDib * pDibSrc, CDib * pDibAlpha);
	BOOL CopyDIBtoDIB2(CDib *pDibSrc,int tx,int ty,int dx,int dy,int orig,int border,int alpha);
	void StretchCopyDIBtoDIB(CDib * pDib,int nFactor);
	void StretchCopyDIBtoDIB(CDib * pDib,int nWidth,int nHeight,int p_x=0,int p_y=0);
	void FillColorUseMask(int x, int y, int MaskTable[15][15],COLORREF rgb);
	void AddPixel(int x,int y,BYTE r,BYTE g,BYTE b);
	void FillColorRect(int x,int y,int R, COLORREF rgb);
	void FillColorRect(int x,int y,int R, BYTE r, BYTE g, BYTE b);
	void FillColor(COLORREF rgb);
	void FillColor(BYTE r,BYTE g,BYTE b);
	void PutPixel(int x,int y,COLORREF rgb);
	COLORREF GetPixelColor(int x,int y);
	void DeleteDIB(void);
	void PutPixel(int x,int y,BYTE r=255,BYTE g=255,BYTE b=255);
	void FillGrayColor(int sx,int sy,int dx,int dy,BYTE key);
	virtual HBITMAP LoadBitmap(HDC hdc, UINT nIDResource);
	HBITMAP GetHandleBitmap(void);
	void* GetPixelAddress(int x,int y,CDib *psrc=NULL);
	HBITMAP CreateNullImage(HDC hdc, int nWidth,int nHeight, int BitCount=24);
	BOOL CopyDIBtoDIB(CDib *pDibSrc,int sx,int sy,int dx,int dy,int x,int y,int nStyle,BYTE r=255,BYTE g=0,BYTE b=255);
	BOOL CopyDIBtoDIB(CDib *pDibSrc,int x,int y,int nStyle,BYTE r=255,BYTE g=0,BYTE b=255);
	CDib(void);
    ~CDib();

    DWORD GetDibSizeImage();
    UINT GetDibWidth();
    UINT GetDibHeight();
    UINT GetDibNumColors();
    LPBITMAPINFOHEADER GetDibInfoHeaderPtr();
    LPBITMAPINFO GetDibInfoPtr();
    BYTE* GetDibBitsPtr();
	virtual HBITMAP LoadBitmapFile(HDC hdc, const char* fileName);
protected:
};

#define	DIB_COPY		0
#define DIB_TRANSPARENT	1
#define DIB_ALPHA		2
#define DIB_ALPHA2		3
#define DIB_BLUR		4
#define DIB_FEATHER 	5

#endif

