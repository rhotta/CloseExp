#include <windows.h>
#include "cdlg.h"

/////////////////////////////

CMDialog::CMDialog(void){
	return ;
}

int
CMDialog::OpenRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent){
	return DialogBoxParam(hInstance,MAKEINTRESOURCE(ResID)
		,hWndParent,(DLGPROC )this->DlgProcD,(LPARAM)this);
}

LRESULT CALLBACK 
CMDialog::DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
	switch (uMsg) {
	case WM_INITDIALOG:
		break;

	case WM_COMMAND:

		switch (LOWORD(wP)){
		case IDOK:
			::EndDialog(hwnd,1);
			break;
		case IDCANCEL:
			::EndDialog(hwnd,0);
			break;

		}
		break;
	}
	return 0;
};