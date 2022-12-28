
/*######################################################################
//  class CMDialog
//	ダイアログ
######################################################################*/

class CMDialog
{
public:
	CMDialog(void);

	int OpenRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent);

	virtual LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);

	HWND GetHandle(void){return mHwnd;}
private:
	void SetHandle(HWND hwnd){
		mHwnd = hwnd;
	}
	static DLGPROC   DlgProcD(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
		CMDialog *cmdlg = (CMDialog *)::GetWindowLong( hwnd, GWL_USERDATA );
		if (cmdlg == NULL) {
			if(uMsg==WM_INITDIALOG) {
				cmdlg=(CMDialog *)lP;
				::SetWindowLong( hwnd, GWL_USERDATA, (LONG)cmdlg );
				cmdlg->SetHandle(hwnd);
				cmdlg->DlgProc(hwnd,uMsg,wP,lP);
			}	
		}else{
				cmdlg->DlgProc(hwnd,uMsg,wP,lP);
		}
		return FALSE;
	}
	HWND mHwnd;
};
