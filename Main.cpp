////////////////////////////////////////////////////////////////////////
//
//FileName: main.cpp
//�A�v���P�[�V����: CloseEXP
//
//����� 01/00/00
//�C���� 01/10/11
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
// WinMain�G���g���֐�
/*//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, 
				   LPSTR lpszArgs, int nWinMode){
	C_CloseExp c_closeexp;
	return c_closeexp.StartUp(hThisInst,lpszArgs);
}
