

/*
メモ
 1.RegisterClass() でクラス名登録 "KNOHLinkClass"
 2.フォントを他のアイテムとあわせる
	HFONT hFont=(HFONT)::SendMessage(GetDlgItem(hwnd,IDC_STATIC_Q),WM_GETFONT,0,0);
	cCLink.SetFont(hFont);
 3.テキスト配置、メッセージ受け取りウインドウなどを設定@WM_INITDIALOG
  	SendDlgItemMessage(IDC_CUSTOM1,WM_HLINKALING_RIGHT,0,0);
	SendDlgItemMessage(IDC_CUSTOM1,WM_HLINKALING_TARGET,(WPARAM)hwnd,IDC_CUSTOM1);

*/

#define WM_HLINKALING_LEFT	WM_USER + 10
#define WM_HLINKALING_RIGHT	WM_USER + 11



//wpにウインドウハンドル指定,lpにID指定で投げる
//クリックされたときのそのハンドルにWM_HLINKALING_TARGETを返す lp にIDが入ってくる
#define WM_HLINKALING_TARGET WM_USER + 199 

typedef struct{
	BOOL fRightAlign;
	HWND hTarget;
	int iIDItem;
}HLINK_WINDATA,*PHLINK_WINDATA;


class C_CTRL_HYPERLINK{




public:
	C_CTRL_HYPERLINK();

	~C_CTRL_HYPERLINK();
//	virtual	LRESULT	WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	virtual BOOL RegisterClass(HINSTANCE hInstance,LPCSTR lpClassName);

	void SetFont(HFONT hFont);



private:

};