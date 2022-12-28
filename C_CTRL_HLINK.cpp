
#include <windows.h>
#include "C_WIN.h"
#include "C_CTRL_HLINK.h"

static HFONT g_hFont = NULL;


LRESULT CALLBACK WndProcHLink(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {

	PHLINK_WINDATA pWinData = (PHLINK_WINDATA)GetWindowLong( hwnd, GWL_USERDATA );
	

		if(!pWinData){
			
			pWinData = (PHLINK_WINDATA)::GlobalAlloc(GMEM_FIXED,sizeof(HLINK_WINDATA));
			SetWindowLong( hwnd, GWL_USERDATA ,(LONG)pWinData);
			pWinData->hTarget = NULL;
		}

        switch (msg) {
		
		case WM_DESTROY:
			{
				::GlobalFree(pWinData);
			}
			break;
		case WM_HLINKALING_LEFT:
			if(pWinData)pWinData->fRightAlign = FALSE;
			break;
		case WM_HLINKALING_RIGHT:
			if(pWinData)pWinData->fRightAlign = TRUE;
			SetWindowLong( hwnd, GWL_USERDATA ,(LONG)pWinData);
			break;	
		case WM_HLINKALING_TARGET:
			{
				pWinData->hTarget = (HWND)wp;
				pWinData->iIDItem = (int)lp;
			}
			break;
        case WM_PAINT:
			{
				char* strText;
				HDC hdc;
				PAINTSTRUCT ps;
				RECT rect;
				int iTextLength;
				char* pPoint;
				iTextLength = GetWindowTextLength(hwnd) + 2;
				strText = (char*)GlobalAlloc(GMEM_FIXED,iTextLength);
				GetWindowText(hwnd , strText , iTextLength);

				pPoint = strText;
				while(*pPoint){
					if(*pPoint=='\t'){*pPoint='\0';break;}
					pPoint++;
				}
				GetClientRect(hwnd , &rect);
					
				hdc = BeginPaint(hwnd , &ps);
					
				SelectObject(hdc,g_hFont);
				SetBkMode(hdc,TRANSPARENT);
				SetTextColor(hdc,RGB(0x00,0x00,0xff));


				DrawText(hdc , strText , -1 , &rect ,(pWinData->fRightAlign ? DT_RIGHT : DT_LEFT) | DT_CALCRECT);
				
/*
				RECT rtwin;

				::GetWindowRect(hwnd,&rtwin);

				int sw = (rtwin.right-rtwin.left) - (rect.right-rect.left);	//k‚ß‚ç‚ê‚é—Ê
				int sy = (rtwin.bottom-rtwin.top) - (rect.bottom-rect.top);

				POINT pt;
				pt.x = rtwin.left;pt.y =  rtwin.top;
				::ScreenToClient(::GetParent(hwnd),&pt);


*/
				if(pWinData->fRightAlign == DT_LEFT){
					::SetWindowPos(hwnd,NULL,0,0,
					rect.right,rect.bottom,SWP_NOZORDER|SWP_NOMOVE);
				}

				GetClientRect(hwnd , &rect);
				int iHeight = DrawText(hdc , strText , -1 , &rect ,pWinData->fRightAlign ? DT_RIGHT : DT_LEFT);
			//	RECT rt;
			//	GetWindowRect(hwnd,&rt);
			//	SetWindowPos(hwnd,NULL,0,0,rect.right-rect.left,
			//		iHeight,SWP_NOZORDER);

				EndPaint(hwnd , &ps);
				
				GlobalFree((HGLOBAL)strText);
			}
			break;
		case WM_LBUTTONUP:
			{
				if(pWinData->hTarget){
					::SendMessage(pWinData->hTarget,WM_HLINKALING_TARGET,(WPARAM)hwnd,(LPARAM)pWinData->iIDItem);
				}else{
					char* strText;
					int iTextLength;
					iTextLength = GetWindowTextLength(hwnd) + 2;
					strText = (char*)GlobalAlloc(GMEM_FIXED,iTextLength);
					GetWindowText(hwnd , strText , iTextLength);
					char *pPoint = strText;
					while(*pPoint){
						if(*pPoint=='\t'){pPoint++;break;}
						pPoint++;
					}

					ShellExecute(NULL,"open" ,pPoint,NULL,NULL,SW_SHOWNORMAL );
					GlobalFree((HGLOBAL)strText);
				}
			}
			break;
        }
        return DefWindowProc(hwnd , msg , wp , lp);
}



C_CTRL_HYPERLINK::C_CTRL_HYPERLINK()
{


}

C_CTRL_HYPERLINK::~C_CTRL_HYPERLINK()
{


}
BOOL C_CTRL_HYPERLINK::RegisterClass(HINSTANCE hInstance,LPCSTR lpClassName)
{
        WNDCLASS winc;

        winc.style              = CS_HREDRAW | CS_VREDRAW;
        winc.lpfnWndProc        = WndProcHLink;
        winc.cbClsExtra = 0;
		winc.cbWndExtra       = sizeof(HLINK_WINDATA);
        winc.hInstance          = hInstance;
        winc.hIcon              = NULL;
        winc.hCursor            = LoadCursor(NULL , MAKEINTRESOURCE(32649));
        winc.hbrBackground      = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
        winc.lpszMenuName       = NULL;
        winc.lpszClassName      = TEXT(lpClassName);

        if (!::RegisterClass(&winc)) return -1;
		return TRUE;
}

void 
C_CTRL_HYPERLINK::SetFont(HFONT hFont){

	if(hFont){
		LOGFONT logfont;
		GetObject(hFont,sizeof(logfont),&logfont);
		logfont.lfUnderline = TRUE;
		g_hFont=CreateFontIndirect(&logfont);
	}else{
		g_hFont = NULL;
	}
}