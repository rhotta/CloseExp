/*
Win32 WindowManage Functions
By Ryusuke Hotta, 2001 Licensed under the MIT license
*/

/////////////////
//filename: c_win.h
//
//
//制作日 01/02/01
//修正日 01/10/11
//
//
/*

+-Ceditwindow
+---Cwindow		通常ウインドウ
+---CMDialog	モーダルダイアログ
+---CMLDialog	モーダレスダイアログ

+-CMDIwindow	MDIウインドウ用

*/
#define ERR_REGCLASS	-1



///////////////////////////////
//既存ウインドウ操作用
class Ceditwindow{
public:
	Ceditwindow();
	Ceditwindow(HWND hwnd);

	void SetTarget(HWND hwnd);//ターゲットの設定

	//ウインドウを表示する
	//nWinMode		表示モード
	void Show(int nWinMode=SW_SHOW);

	//ウインドウハンドルを得る
	HWND GetHandle(void);
#ifndef _WIN32_WCE
	HINSTANCE GetInst(void);
#endif
///////////////////////////////////
//ウインドウの情報操作系

	//ウインドウのサイズを変更する
	void SizeWindow(int w,int h);
#ifndef _WIN32_WCE
	//ウインドウサイズをクライアントサイズ指定で変更する
	void SizeWindowByClient(int w,int h);
#endif
	//ウインドウの移動（絶対座標指定）
	void MoveWindow(int x,int y);
	//ウインドウの移動（相対指定）
	void MoveWindow2(int dx,int dy);

	//中央に移動
	void moveToCenter();

	//幅高さ取得
	int GetClientWidth();
	int GetClientHeight();

	//ウインドウテキストの設定
	void SetWindowText(TCHAR* title);

	//リージョンの設定
	void SetWindowRgn(HRGN hRgn,BOOL bRedraw);//SetWindowRgn

	//最前面設定
	void SetTopMost(BOOL topmost);


	//メッセージ送信
	BOOL PostMessage(UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT SendMessage(UINT Msg,WPARAM wParam,LPARAM lParam);

	//タイマー関連
	UINT SetTimer(UINT nIDEvent,UINT uElapse,TIMERPROC lpTimerFunc=NULL);
	BOOL KillTimer(UINT uIDEvent);
	//
///////////////////////////////////
//ダイアログ関連
	BOOL IsCheckedItem(int nIDDlgItem);
	void SetCheckItem(int nIDDlgItem,BOOL flag);

	BOOL SetDlgItemText(int nIDDlgItem,LPCTSTR lpString);

	HWND GetDlgItem(int nIDDlgItem);
	BOOL GetDlgItemText(int nIDDlgItem,LPTSTR lpString,int nMaxCount );
	UINT GetDlgItemInt(int nIDDlgItem,BOOL *lpTranslated,BOOL bSigned );
	BOOL SetDlgItemInt(int nIDDlgItem,UINT uValue,BOOL bSigned );

	BOOL SetEnableItem(int nIDDlgItem, BOOL enable);

	BOOL AddString(int nIDDlgItem,LPCTSTR lpStr);
	BOOL AddString2(int nIDDlgItem,LPCTSTR lpStr,int iMaxLine);
	void EditControlScrollToBottom(int nIDDlgItem);
	int	DeleteLine(int nIDDlgItem,int Index);//EditBoxの指定行を削除

	LPVOID	GetParam(){return m_pParam;}
protected:
	HWND mHwnd;				//ウインドウハンドル

	LPVOID	m_pParam;		//渡すための任意の値
};

///////////////////////////////
//通常ウインドウ用クラス
class Cwindow:public Ceditwindow
{
public:
	Cwindow();
	~Cwindow();

	//ウインドウクラスの登録、
	//hThisInst		インスタンスハンドル
	//ClassName		クラス名（pwclを省略した場合）
	//lpszMenuName	メニューの使用、省略可 MAKEINTRESOURCE();
	//pwcl			登録するウインドウくらすのポインタ。省略可
#ifndef _UNICODE
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName
		,LPCTSTR lpszMenuName=NULL,WNDCLASSEX* pwcl=NULL,UINT style=0);
#else
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName
		,LPCTSTR lpszMenuName=NULL,WNDCLASS* pwcl=NULL,UINT style=0);
#endif

	//ウインドウの生成
	//hThisInst		同上
	//hwnd			親ウインドウ（デスクトップ＝ HWND_DESKTOP ）
	//dwStyle		ウインドウスタイル、省略可
#ifndef _UNICODE
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,const char* title,
		DWORD dwStyle=WS_OVERLAPPEDWINDOW,DWORD dwExStyle = NULL);
#else
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,const TCHAR* title,
		DWORD dwStyle=WS_OVERLAPPEDWINDOW,DWORD dwExStyle = NULL);
#endif
	//ウインドウを破棄する
	//メモリデバイスがあれば自動的に開放する
	void DestroyWindow();

	//デバイスコンテキストハンドルを得る。ReleaseDC（下）で開放すること
	HDC GetDC();
	//上を呼んだあとに使う
	void ReleaseDC(HDC hdc);

///////////////////////////////////
//メモリデバイス系

	//メモリデバイスコンテキストを製作する
	//製作後はPaint()を使用できる
	int CreateMemDC(int W,int H);
	int CreateMemDC();//<<現在のウインドウサイズを使用

	//上で確保したデバイスコンテキストを削除する（デストラクタでも呼ばれる）
	void DeleteMemDC(void);

	//メモリデバイスコンテキストを得る（生成していた場合）
	HDC GetMemDC();

	//WM_PAINTを処理する（事前にCreateMemDCを実行）
	void Paint(void);

	//メモリデバイスからクライアント領域へビット転送
	void ReDraw(void);

///////////////////////////////////
	//仮想ウインドウプロージャ
	virtual	LRESULT	WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );

	//仮想メッセージループ関数
	virtual int MessageLoop();

private:
protected:
	//コールバック指定用ダミープロージャ
	static LRESULT CALLBACK WndProcD( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam ){
		Cwindow *pWindow = (Cwindow *)::GetWindowLong( hWnd, GWL_USERDATA );

		if (pWindow == NULL) {
			if (iMessage == WM_CREATE) {
				LPCREATESTRUCT lpcs;
				lpcs = (LPCREATESTRUCT) lParam; 
				pWindow = (Cwindow *) lpcs->lpCreateParams;
				::SetWindowLong( hWnd, GWL_USERDATA, (LONG)pWindow );
				pWindow->SetTarget(hWnd);
				return pWindow->WndProc( iMessage, wParam, lParam );
			} else	return ::DefWindowProc( hWnd, iMessage, wParam, lParam );
		} else {
			return pWindow->WndProc( iMessage, wParam, lParam );
		}
	}


	
	TCHAR mClassName[256];	//ウインドウクラス名

	//メモリデバイスコンテキスト用
	HDC mMemDC;				//デバイスコンテキストハンドル
	HBITMAP  mHBit;			//ビットマップハンドル
	int mW;					//幅
	int mH;					//高さ


};




///////////////////////////////
//モーダルダイアログ用クラス
class CMDialog:public Ceditwindow
{
public:
	CMDialog(void);


	//モーダルダイアログを開く
	int Open(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent);
	//上と同じだがダイアログIDを直接指定　（MAKEINTRESOURCEを省略）
	int OpenRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent,LPVOID pParam=NULL);


	//
	virtual LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
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

};




///////////////////////////////
//モーダレスダイアログ用クラス
class CMLDialog:public Ceditwindow
{
public:
	CMLDialog(void);


	//モーダレスダイアログを開く
	BOOL Create(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent);
	//上と同じだがダイアログIDを直接指定　（MAKEINTRESOURCEを省略）
	BOOL CreateRes(HINSTANCE hInstance,WORD ResID,HWND hWndParent,LPVOID pData=NULL);

	void DestroyWindow();

	int MessageLoop();

	virtual LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
		switch (uMsg) {
		case WM_INITDIALOG:
			break;

		case WM_COMMAND:

			switch (LOWORD(wP)){
			case IDOK:
			case IDCANCEL:
				::DestroyWindow(hwnd);
				break;

			}
			break;
		}
		return 0;
	};

private:
	void SetHandle(HWND hwnd){
		mHwnd = hwnd;
	}
	static DLGPROC   DlgProcD(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
		CMLDialog *cmdlg = (CMLDialog *)::GetWindowLong( hwnd, GWL_USERDATA );
		if (cmdlg == NULL) {
			if(uMsg==WM_INITDIALOG) {
				cmdlg=(CMLDialog *)lP;
				::SetWindowLong( hwnd, GWL_USERDATA, (LONG)cmdlg );
				cmdlg->SetHandle(hwnd);
				cmdlg->DlgProc(hwnd,uMsg,wP,lP);
			}
		}else{
				cmdlg->DlgProc(hwnd,uMsg,wP,lP);
		}
		return FALSE;
	}
protected:
//	HWND mHwnd;
};

#ifndef _WIN32_WCE
///////////////////////////////
//MDIウインドウ用
#define IDM_FIRSTCHILD 100
class CMDIwindow{


public:
	CMDIwindow(){;}
	~CMDIwindow(){DestroyMenu(m_hmenuDoc);}

	//ウインドウクラスの登録、
	//hThisInst		インスタンスハンドル
	//ClassName		クラス名
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName,
		LPCTSTR DocWinClassName,UINT style=0);


	//ウインドウプロージャ
	virtual	LRESULT	WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );

	//ドキュメントウインドウプロージャ
	virtual	LRESULT	DocWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

	//フレームウインドウとクライアントウインドウを作る
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,TCHAR* title,
		LPTSTR lpszMenuName,LPTSTR lpszDocMenuName,
		int wmenu_1=0,int wmenu_2=0);

	//ドキュメントウインドウを作る
	virtual HWND CreateDocWin(LPTSTR title);

	//アクティブなドキュメントウインドウにメッセージを送る
	//フレームウインドウのWM_COMMANDのデフォルトなどで使う
	BOOL SendMessageToActiveDocWin(UINT iMessage, WPARAM wParam, LPARAM lParam);

	//ドキュメントウインドウを全て閉じる
	void CloseAllDocWindow();

	//並べて表示
	void DocWnd_TILE(){SendMessage(m_hClient, WM_MDITILE, 0, 0);};
	//重ねて表示
	void DocWnd_CASCADE(){SendMessage(m_hClient, WM_MDICASCADE, 0, 0);};
	//アイコンの整理
	void DocWnd_ARRANGE(){SendMessage(m_hClient, WM_MDIICONARRANGE, 0, 0);};

	HINSTANCE GetInst(){return m_hThisInst;}


	//クライアントを作る。呼ぶ必要なし
	HWND CreateClient(HWND hwnd);

	//メッセージループ
	virtual int MessageLoop();

	HMENU GetMenu();
	HWND GetFrameHandle(){return m_hFrame;}
	HWND GetClientHandle(){return m_hClient;}
	int EVID_of_LastCreateListWin;	//一時保存用　リスト生成時にプロージャへ渡す
private:
	static LRESULT CALLBACK WndProcD( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam ){
		CMDIwindow *pWindow = (CMDIwindow *)::GetWindowLong( hWnd, GWL_USERDATA );

		if (pWindow == NULL) {
			if (iMessage == WM_CREATE) {
				LPCREATESTRUCT lpcs;
				lpcs = (LPCREATESTRUCT) lParam; 
				pWindow = (CMDIwindow *) lpcs->lpCreateParams;
				::SetWindowLong( hWnd, GWL_USERDATA, (LONG)pWindow );



				pWindow->CreateClient(hWnd);
								
		//		return pWindow->WndProc( iMessage, wParam, lParam );

				return 0;
			} else return DefWindowProc( hWnd, iMessage, wParam, lParam );;
				//return ::DefFrameProc( hWnd,pWindow->GetClientHandle()
				//, iMessage, wParam, lParam );
		} else {
		
			return pWindow->WndProc( iMessage, wParam, lParam );
		}
	}
	static LRESULT CALLBACK DocWndProcD( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam ){
		CMDIwindow *pWindow = (CMDIwindow *)::GetWindowLong( hWnd, GWL_USERDATA );

		if (pWindow == NULL) {
			if (iMessage == WM_CREATE ) {
				LPCREATESTRUCT lpcs;
				LPMDICREATESTRUCT mdic;
				lpcs = (LPCREATESTRUCT) lParam; 
				mdic = (LPMDICREATESTRUCT)lpcs->lpCreateParams;
				pWindow = (CMDIwindow *) mdic->lParam;
				::SetWindowLong( hWnd, GWL_USERDATA, (LONG)pWindow );
				return pWindow->DocWndProc(hWnd, iMessage, wParam, lParam );
		//		return 0;
			} else	return ::DefMDIChildProc( hWnd, iMessage, wParam, lParam );
		} else {
			return pWindow->DocWndProc(hWnd, iMessage, wParam, lParam );
		}
	}
	static BOOL CALLBACK CloseAllProc(HWND hWnd, LPARAM lp)
	{
		SendMessage(GetParent(hWnd), WM_MDIDESTROY, (WPARAM)hWnd, 0);
			return TRUE;
	}
protected:
	HMENU m_hmenu;
	HMENU m_hmenuWnd;
	HMENU m_hmenuDoc;
	HMENU m_hmenuDocWnd;



	TCHAR mClassName[256];	//ウインドウクラス名
	TCHAR mDocWinClassName[256];	//ウインドウクラス名
	HINSTANCE m_hThisInst;
	HWND m_hClient;
	HWND m_hFrame;
};
#endif //end #ifndef _WIN32_WCE













