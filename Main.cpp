////////////////////////////////////////////////////////////////////////
//
//FileName: main.cpp
//アプリケーション: CloseEXP
//
//制作日 01/00/00
//修正日 01/10/11
//
//(c)2001 RyusukeHotta

#include <windows.h>


#include "resource.h"
#include "main.h"

#include "c_win.h"
#include "c_ctrl_hlink.h"
#include "closeexp.h"



/*//////////////////////////////////////////////////////////////////////
// WinMain()
// WinMainエントリ関数
/*//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, 
				   LPSTR lpszArgs, int nWinMode){
	C_CloseExp c_closeexp;
	return c_closeexp.StartUp(hThisInst,lpszArgs);
}
