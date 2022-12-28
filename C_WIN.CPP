/*
Win32 WindowManage Functions
By Ryusuke Hotta, 2001 Licensed under the MIT license
*/


/////////////////
//filename: c_win.cpp
//
//Windows、ウインドウに関する基本操作用クラス
//
//
//制作日 01/02/01
//修正日 01/10/11
//



/*
MDIウインドウ関連未完成
  */
  
#define _CRT_SECURE_NO_DEPRECATE 

#include <windows.h>
#include <mbstring.h>
#include <tchar.h> 
#include "c_win.h"


/*********************************************************
*	Ceditwindow()
*	コントラクタ
*********************************************************/
Ceditwindow::Ceditwindow(){

	return;
}
/*********************************************************
*	Ceditwindow()
*	コントラクタ
*********************************************************/
Ceditwindow::Ceditwindow(HWND hwnd){
	mHwnd = hwnd;
	return;
}

/*********************************************************
*	SetTarget()
*	ウインドウハンドルを指定（既存ウインドウ操作用）
*********************************************************/
void
Ceditwindow::SetTarget(HWND hwnd){
	this->mHwnd = hwnd;
}

/*********************************************************
*	GetHandle()
*	ウインドウハンドルを取得
*********************************************************/
HWND
Ceditwindow::GetHandle(void){
	return mHwnd;
}

/*********************************************************
*	GetInst()
*	インスタンスハンドルを取得
*********************************************************/
#ifndef _WIN32_WCE
HINSTANCE
Ceditwindow::GetInst(void){
	return (HINSTANCE)GetWindowLong(mHwnd,GWL_HINSTANCE);
}
#endif
/*********************************************************
*	Show()
*	ウインドウを表示
*********************************************************/
void
Ceditwindow::Show(int nWinMode){

	ShowWindow(mHwnd, nWinMode);
	UpdateWindow(mHwnd);
	return;
}

/*********************************************************
*	SizeWindow()
*	ウインドウのサイズを変更
*********************************************************/
void 
Ceditwindow::SizeWindow(int w,int h){
	::SetWindowPos(mHwnd,NULL,0,0,w,h,SWP_NOMOVE|SWP_NOZORDER);
}

/*********************************************************
*	SizeWindowByClient()
*	クライアントサイズが指定サイズになるようにリサイズ
*********************************************************/
#ifndef _WIN32_WCE
void 
Ceditwindow::SizeWindowByClient(int w,int h ){
	RECT r;
	r.left=0;r.top=0;
	r.right=w;r.bottom=h;
	if(GetMenu(mHwnd)==NULL){
		::AdjustWindowRect(&r,(DWORD)GetWindowLong(mHwnd,GWL_STYLE),FALSE);
	}else{
		::AdjustWindowRect(&r,(DWORD)GetWindowLong(mHwnd,GWL_STYLE),TRUE);

	}	
	::SetWindowPos(mHwnd,NULL,0,0,r.right-r.left,r.bottom-r.top,SWP_NOMOVE|SWP_NOZORDER);
}
#endif
/*********************************************************
*	MoveWindow()
*	ウインドウを移動（絶対座標）
*********************************************************/
void
Ceditwindow::MoveWindow(int x,int y){
	::SetWindowPos(mHwnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

/*********************************************************
*	MoveWindow2()
*	ウインドウを移動（相対）
*********************************************************/
void
Ceditwindow::MoveWindow2(int dx,int dy){
	RECT rt;
	GetWindowRect(mHwnd,&rt);
	::SetWindowPos(mHwnd,NULL,rt.left+dx,rt.top+dy,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

/*********************************************************
*	SetWindowText()
*	ウインドウテキストの設定
*********************************************************/
void 
Ceditwindow::SetWindowText(TCHAR* title){
	::SetWindowText(mHwnd,title);
}

/*********************************************************
*	moveToCenter()
*	ウインドウを画面の中心に移動
*********************************************************/
void Ceditwindow::moveToCenter(){
	RECT rt,rt2;
	::GetWindowRect(this->mHwnd,&rt2);
	::GetWindowRect(::GetDesktopWindow(),&rt);
	this->MoveWindow((rt.right+rt.left)/2-(rt2.right-rt2.left)/2,
		(rt.bottom-rt.top)/2-(rt2.bottom-rt2.top)/2);
	return;
}
/*********************************************************
*	GetClientWidth()
*	ウインドウを幅を取得
*********************************************************/
int Ceditwindow::GetClientWidth(){
	RECT rt;
	::GetClientRect(mHwnd,&rt);
	return rt.right-rt.left;
}
/*********************************************************
*	GetClientHeight()
*	ウインドウを高さを取得
*********************************************************/
int Ceditwindow::GetClientHeight(){
	RECT rt;
	::GetClientRect(mHwnd,&rt);
	return rt.bottom-rt.top;
}

/*********************************************************
*	SetWindowRgn()
*	リージョン設定
*********************************************************/
void
Ceditwindow::SetWindowRgn(HRGN hRgn,BOOL bRedraw){
	
	::SetWindowRgn(mHwnd,hRgn,bRedraw);
}

/*********************************************************
*	SetTopMost()
*	最前面に設定
*********************************************************/
void
Ceditwindow::SetTopMost(BOOL topmost){
	::SetWindowPos(mHwnd,topmost?HWND_TOPMOST:HWND_NOTOPMOST,
		0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	return;
}

/*********************************************************
*	PostMessage()
*	メッセージをポスト
*********************************************************/
BOOL
Ceditwindow::PostMessage(UINT Msg,WPARAM wParam,LPARAM lParam){
	return ::PostMessage(mHwnd,Msg,wParam,lParam);
}

/*********************************************************
*	SendMessage()
*	メッセージを送信
*********************************************************/
LRESULT
Ceditwindow::SendMessage(UINT Msg,WPARAM wParam,LPARAM lParam){
	return ::SendMessage(mHwnd,Msg,wParam,lParam);
}
/*********************************************************
*	SetTimer()
*	タイマーを設定
*********************************************************/
UINT
Ceditwindow::SetTimer(UINT nIDEvent,UINT uElapse,TIMERPROC lpTimerFunc){
	return ::SetTimer(mHwnd,nIDEvent,uElapse,lpTimerFunc);
}

/*********************************************************
*	SetTimer()
*	タイマーを設定
*********************************************************/
BOOL
Ceditwindow::KillTimer(UINT nIDEvent){
	return ::KillTimer(mHwnd,nIDEvent);
}

/*##############################################################################
  CLASS Ceditwindow		//ダイアログ
##############################################################################*/

/*********************************************************
*	IsCheckedItem()
*	アイテムのチェック状態を取得
*********************************************************/
BOOL Ceditwindow::IsCheckedItem(int nIDDlgItem){
	return SendDlgItemMessage(mHwnd,nIDDlgItem,BM_GETCHECK,0,0);

}

/*********************************************************
*	SetCheckItem()
*	アイテムのチェック状態を設定
*********************************************************/
void Ceditwindow::SetCheckItem(int nIDDlgItem,BOOL flag){
	SendDlgItemMessage(mHwnd,nIDDlgItem,BM_SETCHECK,flag?BST_CHECKED:BST_UNCHECKED,0);

};

/*********************************************************
*	SetDlgItemText()
*	アイテムのチェック状態を設定
*********************************************************/
BOOL Ceditwindow::SetDlgItemText(int nIDDlgItem,LPCTSTR lpString){
	return ::SetDlgItemText(mHwnd,nIDDlgItem,lpString);
};


BOOL Ceditwindow::SetEnableItem(int nIDDlgItem, BOOL enable)
{
	return ::EnableWindow(this->GetDlgItem(nIDDlgItem),enable);
}
/*********************************************************
*	GetDlgItem()
*	アイテム取得
*********************************************************/
HWND Ceditwindow::GetDlgItem(int nIDDlgItem){
	return ::GetDlgItem(mHwnd,nIDDlgItem);
};
/*********************************************************
*	GetDlgItemText()
*	テキスト取得
*********************************************************/
BOOL Ceditwindow::GetDlgItemText(int nIDDlgItem,LPTSTR lpString,int nMaxCount){
	return ::GetDlgItemText(mHwnd,nIDDlgItem,lpString,nMaxCount);
};
/*********************************************************
*	SetDlgItemInt()
*	整数値設定
*********************************************************/
BOOL Ceditwindow::SetDlgItemInt(int nIDDlgItem,UINT uValue,BOOL bSigned ){
	return ::SetDlgItemInt(mHwnd,nIDDlgItem,uValue,bSigned);
};
/*********************************************************
*	GetDlgItemInt()
*	整数値取得
*********************************************************/
UINT Ceditwindow::GetDlgItemInt(int nIDDlgItem,BOOL *lpTranslated,BOOL bSigned ){
	return ::GetDlgItemInt(mHwnd,nIDDlgItem,lpTranslated,bSigned);
};
/*********************************************************
*	AddString()
*	文字列をエディットボックスに追加
*********************************************************/
BOOL Ceditwindow::AddString(int nIDDlgItem,LPCTSTR lpStr){
	HWND hEdit = ::GetDlgItem(mHwnd,nIDDlgItem);
	int nums = ::GetWindowTextLength(hEdit);
	::SendMessage(hEdit,EM_SETSEL,nums,nums);
	::SendMessage(hEdit,EM_REPLACESEL,0,(LPARAM)lpStr);


	return TRUE;
}
BOOL Ceditwindow::AddString2(int nIDDlgItem,LPCTSTR lpStr,int iMaxLine)
{
	HWND hEdit = ::GetDlgItem(mHwnd,nIDDlgItem);
	int iFlagLines = ::SendMessage(hEdit,EM_GETLINECOUNT,0,0);
	::SendMessage(hEdit,WM_SETREDRAW,FALSE,0);
	for(;;){
		if(iFlagLines < iMaxLine+1) break;
		DeleteLine(nIDDlgItem,0);
		iFlagLines--;
	}
	
	
	
	::SendMessage(hEdit,WM_SETREDRAW,TRUE,0);

	AddString(nIDDlgItem,lpStr);

	

	return TRUE;
}

void Ceditwindow::EditControlScrollToBottom(int nIDDlgItem)
{
	HWND hEdit = ::GetDlgItem(mHwnd,nIDDlgItem);
	int num = ::SendMessage(hEdit,WM_GETTEXTLENGTH ,(WPARAM)0,(LPARAM)0);
	::SendMessage(hEdit,EM_SETSEL,num,num);
	
	::SendMessage(hEdit,EM_SCROLLCARET ,(WPARAM)0,(LPARAM)0);

	InvalidateRect(hEdit,NULL,TRUE );
	UpdateWindow(hEdit);
	
}


/*********************************************************
*	DeleteLine()
*	//エディットボックスの指定行を削除する
*********************************************************/
int Ceditwindow::DeleteLine(int nIDDlgItem,int Index)
{
	HWND hEdit = ::GetDlgItem(mHwnd,nIDDlgItem);
    int SelStart, SelEnd;
	SelStart = ::SendMessage(hEdit,EM_LINEINDEX, Index, 0);
    if (SelStart >= 0)
	{
		SelEnd = ::SendMessage(hEdit,EM_LINEINDEX, Index + 1, 0);
		if (SelEnd < 0)
			SelEnd = SelStart + ::SendMessage(hEdit,EM_LINELENGTH, SelStart, 0);
		::SendMessage(hEdit,EM_SETSEL, SelStart, SelEnd);
		::SendMessage(hEdit,EM_REPLACESEL, 0, (LPARAM)"");
		return (SelEnd - SelStart);
	}
	return 0;
}

/*##############################################################################
  CLASS Cwindow	
##############################################################################*/

/*********************************************************
*	Cwindow()
*	コントラクタ
*********************************************************/
Cwindow::Cwindow(){
	mMemDC =NULL; mHBit =NULL;
	mHwnd = NULL;
	return ;
}

/*********************************************************
*	~Cwindow()
*	デストラクタ
*********************************************************/
Cwindow::~Cwindow(){
	DeleteMemDC();
	return ;
}


/*********************************************************
*	WndProc()
*	ウインドウプロージャ
*********************************************************/
LRESULT	
Cwindow::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) {
	switch(iMessage){
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		Paint();
		break;
	}
	return DefWindowProc( mHwnd, iMessage, wParam, lParam );
}

/*********************************************************
*	RegisterClass()
*	ウインドウクラスを登録する
*********************************************************/
int
#ifndef UNICODE
Cwindow::RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName,LPCTSTR lpszMenuName,WNDCLASSEX* pwcl,UINT style)
#else
Cwindow::RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName,LPCTSTR lpszMenuName,WNDCLASS* pwcl,UINT style)
#endif
{
	if(mHwnd){
		if(::IsWindow(mHwnd))
			this->DestroyWindow();
		
	}

		
#ifndef UNICODE
	strcpy(mClassName,ClassName);
	if(pwcl == NULL){
		WNDCLASSEX wcl;
		wcl.cbSize = sizeof(WNDCLASSEX); 
		wcl.cbWndExtra	= sizeof(Cwindow*);
		wcl.cbClsExtra	= 0;
		wcl.hInstance = hThisInst;
		wcl.lpszClassName = ClassName;
		wcl.lpfnWndProc = WndProcD;
	wcl.style = (style) ? style
						  :CS_HREDRAW | CS_VREDRAW;
		wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 
		wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
		wcl.lpszMenuName = lpszMenuName; 
		wcl.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH); 
		if(!::RegisterClassEx(&wcl)) return ERR_REGCLASS;
	}else{
		pwcl->lpfnWndProc = WndProcD;
		if(!::RegisterClassEx(pwcl)) return ERR_REGCLASS;

	}
#else
	wcscpy(mClassName,ClassName);
	
	if(pwcl == NULL){
		WNDCLASS wcl;
	//	wcl.cbSize = sizeof(WNDCLASS); 
		wcl.cbWndExtra	= sizeof(Cwindow*);
		wcl.cbClsExtra	= 0;
		wcl.hInstance = hThisInst;
		wcl.lpszClassName = ClassName;
		wcl.lpfnWndProc = WndProcD;
		wcl.style = 0; 
		wcl.hIcon = NULL;//LoadIcon(NULL, IDI_APPLICATION);
	//	wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 
		wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
		wcl.lpszMenuName = lpszMenuName; 
		wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); 
		if(!::RegisterClass(&wcl)) return ERR_REGCLASS;
	}else{
		pwcl->lpfnWndProc = WndProcD;
		if(!::RegisterClass(pwcl)) return ERR_REGCLASS;

	}
#endif
	return 0;
}

/*********************************************************
*	Create()
*	ウインドウを作る
*********************************************************/
int
Cwindow::Create(HINSTANCE hThisInst,HWND hWndParent,const TCHAR* title,DWORD dwStyle,DWORD dwExStyle){

	mHwnd = CreateWindowEx(
	dwExStyle,
	mClassName, 
	title,
	dwStyle, 
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	hWndParent,
	NULL,
	hThisInst, 
	(LPSTR)this
	);


	return 0;
}

/*********************************************************
*	DestroyWindow()
*	ウインドウを破棄
*********************************************************/
void
Cwindow::DestroyWindow(void){
	this->DeleteMemDC();
	if(IsWindow(mHwnd))
		::DestroyWindow(mHwnd);
	mHwnd = NULL;
}

/*********************************************************
*	GetDC()
*	クライアントデバイスコンテキストを取得
*********************************************************/
HDC Cwindow::GetDC(){
	return ::GetDC(mHwnd);
}

/*********************************************************
*	GetMemDC()
*	仮想デバイスコンテキストを取得（生成済みの場合）
*********************************************************/
HDC Cwindow::GetMemDC(){
	return mMemDC;
}

/*********************************************************
*	ReleaseDC()
*	デバイスコンテキストを解放
*********************************************************/
void
Cwindow::ReleaseDC(HDC hdc){
	::ReleaseDC(mHwnd,hdc);
}

/*********************************************************
*	CreateMemDC()
*	メモリデバイスコンテキストを生成
*********************************************************/
int 
Cwindow::CreateMemDC(){
	RECT rt;
	::GetWindowRect(mHwnd,&rt);
	return this->CreateMemDC(rt.right-rt.left,rt.bottom-rt.top);

}
int 
Cwindow::CreateMemDC(int W,int H){
	HDC hdc;
	mW = W;mH = H;
	hdc = ::GetDC(mHwnd);
	mMemDC = ::CreateCompatibleDC(hdc);
	mHBit = ::CreateCompatibleBitmap(hdc,mW,mH);
	::ReleaseDC(mHwnd,hdc);
	::SelectObject(mMemDC, mHBit);
	HPEN hPen = (HPEN)::GetStockObject(WHITE_PEN);
	HBRUSH hBrush = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	::SelectObject(mMemDC, hPen);
	::SelectObject(mMemDC, hBrush);
	::Rectangle(mMemDC,0,0,W,H);

	return 0;

}

/*********************************************************
*	DeleteMemDC()
*	メモリデバイスコンテキストを破棄
*********************************************************/
void 
Cwindow::DeleteMemDC(void){
	if(mMemDC!=NULL)::DeleteDC(mMemDC);
	if(mHBit!=NULL)::DeleteObject (mHBit);
	mMemDC = NULL;
	mHBit = NULL;
}

/*********************************************************
*	Paint()
*　再描写メッセージを処理
*********************************************************/
void 
Cwindow::Paint(void){
	HDC hdc;
	PAINTSTRUCT ps;
	if(mMemDC != NULL){
		hdc = BeginPaint(mHwnd,&ps);
		::BitBlt(hdc,ps.rcPaint.left,ps.rcPaint.top,
				ps.rcPaint.right-ps.rcPaint.left,
				ps.rcPaint.bottom-ps.rcPaint.top,
				mMemDC,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
		::EndPaint(mHwnd,&ps);
	}

}

/*********************************************************
*	ReDraw()
*　メモリデバイスコンテキストからクライアントへ転送
*********************************************************/
void 
Cwindow::ReDraw(){
	HDC hdc;
	hdc=GetDC();
	::BitBlt(hdc,0,0,mW,mH,mMemDC,0,0,SRCCOPY);
//	GdiFlush();
	ReleaseDC(hdc);

}



/*********************************************************
*	MessageLoop()
*　簡易メッセージループ
*********************************************************/
int
Cwindow::MessageLoop(){
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg); 

	}
	return (int)msg.wParam;
}

/*##############################################################################
  CMDialog class
##############################################################################*/
/*********************************************************
*	CMDialog()
*　コントラクタ
*********************************************************/
CMDialog::CMDialog(void){
	return ;
}

/*********************************************************
*	Open()
*　生成
*********************************************************/
int
CMDialog::Open(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent){
	return DialogBoxParam(hInstance,lpTemplate
		,hWndParent,(DLGPROC )this->DlgProcD,(LPARAM)this);

}

/*********************************************************
*	OpenRes()
*　リソースから生成（省略）
*********************************************************/
int
CMDialog::OpenRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent,LPVOID pParam){
	m_pParam = pParam;
	return DialogBoxParam(hInstance,MAKEINTRESOURCE(ResID)
		,hWndParent,(DLGPROC )this->DlgProcD,(LPARAM)this);
}

#ifndef _WIN32_WCE
//#ifndef UNICODE //未実装
/*##############################################################################
  CMLDialog class
##############################################################################*/
/*********************************************************
*	CMLDialog()
*　コントラクタ
*********************************************************/
CMLDialog::CMLDialog(void){
	return ;
}

/*********************************************************
*	Create()
*　生成
*********************************************************/
BOOL
CMLDialog::Create(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent){
	mHwnd= CreateDialogParam(hInstance,lpTemplate
		,hWndParent,(DLGPROC )this->DlgProcD,(LPARAM)this);
	if(!mHwnd) return FALSE;
	else return TRUE;
}
/*********************************************************
*	CreateRes()
*　リソースから生成
*********************************************************/
BOOL
CMLDialog::CreateRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent,LPVOID pData){
	m_pParam = pData;
	mHwnd= CreateDialogParam(hInstance,MAKEINTRESOURCE(ResID)
		,hWndParent,(DLGPROC )this->DlgProcD,(LPARAM)this);
	if(!mHwnd) return FALSE;
	else return TRUE;
}

/*********************************************************
*	DestroyWindow()
*　破棄
*********************************************************/
void
CMLDialog::DestroyWindow(){

	::DestroyWindow(mHwnd);
}
/*********************************************************
*	MessageLoop()
*　簡易メッセージループ
*********************************************************/
int
CMLDialog::MessageLoop(){
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{

		if(!IsDialogMessage(mHwnd,&msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}

	}
	return (int)msg.wParam;
}
/*##############################################################################
  CMDIwindow class
##############################################################################*/
/*********************************************************
*	RegisterClass()
*　クラス登録
*********************************************************/
int
CMDIwindow::RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName,
						  LPCTSTR DocWinClassName,UINT style){

	m_hThisInst = hThisInst;
	_tcscat(mClassName,ClassName);
	_tcscat(mDocWinClassName,DocWinClassName);
	WNDCLASSEX wcl;
	wcl.cbSize = sizeof(WNDCLASSEX); 
	wcl.cbWndExtra	= sizeof(CMDIwindow*);
	wcl.cbClsExtra	= 0;
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = ClassName;
	wcl.lpfnWndProc = WndProcD;
	wcl.style = (style) ? style
						:CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wcl.lpszMenuName = NULL; 
	wcl.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1); 
	if(!RegisterClassEx(&wcl)) return ERR_REGCLASS;
/*
	wcl.cbSize = sizeof(WNDCLASSEX); 
	wcl.cbWndExtra	= sizeof(CMDIwindow*);
	wcl.cbClsExtra	= 0;
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = DocWinClassName;
	wcl.lpfnWndProc = DocWndProcD;
	wcl.style = CS_HREDRAW | CS_VREDRAW;; 
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wcl.lpszMenuName = NULL; 
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	if(!RegisterClassEx(&wcl)) return ERR_REGCLASS;*/
	return 0;
}

/*********************************************************
*	Create()
*　生成
*********************************************************/
int CMDIwindow::Create(HINSTANCE hThisInst,HWND hWndParent,TCHAR* title,LPTSTR lpszMenuName,
					   LPTSTR lpszDocMenuName,
					   int wmenu_1,int wmenu_2){
	m_hThisInst = hThisInst;
	m_hmenu  = LoadMenu(hThisInst, MAKEINTRESOURCE(lpszMenuName));
	m_hmenuDoc  = LoadMenu(hThisInst, MAKEINTRESOURCE(lpszDocMenuName));
	m_hmenuWnd =GetSubMenu(m_hmenu, 0);
	m_hmenuDocWnd = GetSubMenu(m_hmenuDoc, 0);

    m_hFrame = CreateWindow(
        mClassName,
        title,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        m_hmenu,
        hThisInst,
        (LPSTR)this);


	return 0;
}

//#include "C_WINGUI.H"
//CWinGUI gui;
/*********************************************************
*	CreateClient()
*　MDIクライアントウインドウを生成する
*********************************************************/
HWND CMDIwindow::CreateClient(HWND hwnd){
	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu =  m_hmenuWnd;

	ccs.idFirstChild = IDM_FIRSTCHILD;
	m_hClient = CreateWindow(
		_T("MDICLIENT"),
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		0, 0, 0, 0,
		hwnd,
		(HMENU)1,
		m_hThisInst,
		   (LPSTR)&ccs);
/*
	if(!m_hClient) {
		
			Beep(1000,1000);
			MessageBox(NULL,"aa","aa",NULL);
		gui.MessageLastError();
	}*/
	return m_hClient;
}


/*********************************************************
*	GetMenu()
*　
*********************************************************/
HMENU CMDIwindow::GetMenu(){
	return this->m_hmenu;
}

/*********************************************************
*	CreateDocWin()
*　ドキュメントウインドウを生成する
*********************************************************/
HWND CMDIwindow::CreateDocWin(LPTSTR title){
    MDICREATESTRUCT mdic;

	mdic.szClass = mDocWinClassName;
	mdic.szTitle = title;
	mdic.hOwner = m_hThisInst;
	mdic.x = CW_USEDEFAULT;
	mdic.y = CW_USEDEFAULT;
	mdic.cx = CW_USEDEFAULT;
	mdic.cy = CW_USEDEFAULT;
	mdic.style = 0;
	mdic.lParam = (LPARAM)this;
	return (HWND)SendMessage(m_hClient, WM_MDICREATE, 0,(LPARAM)&mdic);
//return NULL;
}

/*********************************************************
*	WndProc()
*　ウインドウプローじゃ
*********************************************************/
LRESULT	
CMDIwindow::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) {

		
		if (iMessage == WM_DESTROY)  {
		//	DestroyWindow();
			PostQuitMessage(0);
		} 
		return DefWindowProc( m_hFrame, iMessage, wParam, lParam );
}

/*********************************************************
*	SendMessageToActiveDocWin()
*　全てのドキュメントウインドウにメッセージを送信する
*********************************************************/
BOOL CMDIwindow::SendMessageToActiveDocWin( UINT iMessage, WPARAM wParam, LPARAM lParam){
	
	HWND hChild = (HWND)SendMessage(m_hClient, WM_MDIGETACTIVE, 0, 0);
	if (IsWindow(hChild)){
		SendMessage(hChild, iMessage, wParam, lParam);
		return TRUE;
	}else
		return FALSE;

}

/*********************************************************
*	CloseAllDocWindow()
*　全てのドキュメントウインドウを閉じる
*********************************************************/
void CMDIwindow::CloseAllDocWindow(){
//int alll;
//	::EnumChildWindows(m_hClient, (void)&CloseAllProc, 0);
}


/*********************************************************
*	DocWndProc()
*　ドキュメントウインドウのウインドウプロージャ
*********************************************************/
LRESULT	
CMDIwindow::DocWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam ) {
//
		if (iMessage ==   WM_MDIACTIVATE)  {
	//		Beep(1000,1000);
	
            if (lParam == (LPARAM)hWnd)
                SendMessage(m_hClient, WM_MDISETMENU, (WPARAM)m_hmenuDoc, (LPARAM)m_hmenuDocWnd);
            if (lParam != (LPARAM)hWnd)
                SendMessage(m_hClient, WM_MDISETMENU, (WPARAM)m_hmenu, (LPARAM)m_hmenuWnd);
            DrawMenuBar(m_hFrame);
			return 0;
		} 
		return DefMDIChildProc( hWnd, iMessage, wParam, lParam );




}
/*********************************************************
*	MessageLoop()
*　簡易メッセージループ
*********************************************************/
int
CMDIwindow::MessageLoop(){
	MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateMDISysAccel(m_hClient, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	return (int)msg.wParam;
}



//#endif //end #ifndef _WIN32_WCE
#endif





