////////////////////////////////////////////////////////////////////////
//
//FileName: closeexp.cpp
//アプリケーション: CloseEXP
//
//制作日 01/--/--
//修正日 01/10/11
//
//(c)2001 


/*##############################################################################
  include
##############################################################################*/

#include <windows.h>
#include <shlobj.h>
#include <psapi.h>

#include "c_win.h"
#include "c_path.h"
#include "c_ctrl_hlink.h"
#include "resource.h"
#include "closeexp.h"


//後方から文字列を比較
int strcmp_behind(const char* shorterstring, const char* longerstring)
{
	const char* p = longerstring + (strlen(longerstring) - strlen(shorterstring));
	return strcmp(shorterstring, p);
}


/*##############################################################################
  グローバル
##############################################################################*/

/*//////////////////////////////////////////////////////////////////////
// EnumWindowsProcDammy()
// ウインドウ列挙用、コールバック関数のダミー
/*//////////////////////////////////////////////////////////////////////
BOOL CALLBACK 
EnumWindowsProcDammy(HWND hwnd,LPARAM lParam)
{
	C_CloseExp *pc_closeexp = (C_CloseExp*)lParam;
	return pc_closeexp->EnumWindowsProc(hwnd);
}


/*##############################################################################
  CLASS C_CloseExp
##############################################################################*/

/*//////////////////////////////////////////////////////////////////////
// C_CloseExp()
// C_CloseExpコントラクタ
/*//////////////////////////////////////////////////////////////////////
C_CloseExp::C_CloseExp()
{
	m_MainDlgCreated = FALSE;

	/*設定、初期値代入*/
	m_tagSetting.fTopmost =  TRUE; //常に手前に表示
	m_tagSetting.fFullSize = FALSE;
	m_tagSetting.tagTarget.Explorer = TRUE;
	m_tagSetting.tagTarget.EdgeChrome =FALSE;
	m_tagSetting.tagTarget.Firefox = FALSE;
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		m_tagSetting.tagTarget.APEND[i].fTarget = FALSE;
	}
	m_tagSetting.fKepp1St = FALSE;

	m_tagSetting.iSleepTime = 50; //閉じるごとにこの値だけ待つ

	m_tagSetting.iWinLeft = -1;
	m_tagSetting.iWinTop = -1;

	m_tagSetting.iHotKeyMode = MODE_HOTKEY_NORMAL;
	m_tagSetting.fNotBackground = FALSE;

	m_fRegistHotkey = FALSE;
}
/*//////////////////////////////////////////////////////////////////////
// StartUp()
// エントリﾎﾟｲﾝﾄ（WinMainで呼び出し）1度のみ呼び出される
/*//////////////////////////////////////////////////////////////////////
int
C_CloseExp::StartUp(HINSTANCE hThisInst,LPSTR lpszArgs)
{
	BOOL fFirstRun;
	m_hHinst =hThisInst;
	mc_maindlg.SetCloseExpClassAddress((void*)this);

	/*カスタムコントロール用*/
	cCLink.RegisterClass(hThisInst,"KNOHLinkClass");


	/*設定の読み込み*/
	fFirstRun = this->LoadSettings();
		
	
	if(m_tagSetting.fNotBackground){	/*非常駐モードの場合*/
		/*ダイアログ表示*/
		ShowMainDialog();

		/*設定の保存*/
		SaveSettings();

		return 0;
	}else{								/*通常（常駐）モードの場合*/

		/*MUTEXの登録*/
		HANDLE hMutex=::CreateMutex(NULL,TRUE,STR_CLEXP_MUTEX);
		if(!hMutex) return 0;
		if(::GetLastError() == ERROR_ALREADY_EXISTS){
			HWND hPrev  = ::FindWindow(STR_CLEXP_CLASS,STR_CLEXP_WINTEXT);
			if(hPrev){
				::PostMessage(hPrev,WM_CLEXP_SHOWDLG,0,0);
				return 0; /*2重起動はしない*/
			}
		}

		/*ダミーウインドウの生成*/
		mc_dammywindow.SetCloseExpClassAddress((void*)this);
		mc_dammywindow.RegisterClass(
			hThisInst,STR_CLEXP_CLASS
			);
		mc_dammywindow.Create(hThisInst,NULL,STR_CLEXP_WINTEXT);


		/*ホットキーの設定*/
		this->RegisterHotkeys();

		/*タスクトレイアイコン登録*/
		this->TasktrayAdd();

		/*最初の起動ならばダイアログ表示*/
		if(fFirstRun)//this->ShowMainDialog();
			OnAboutCloseExp(HWND_DESKTOP);
	}

	/*メッセージループ*/
	return mc_dammywindow.MessageLoop();


}
/*//////////////////////////////////////////////////////////////////////
// CloseTargetWindows()
// ウインドウを閉じる
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::CloseTargetWindows(HWND hwnd)
{
	BOOL fClose = FALSE;
	if (!m_bCountOnly) {
						
		if (m_bKeep1st) {
				
			m_bKeep1st = FALSE;				
		}
		else {
				
			if (m_bMinisize) this->MinisizeWindow(hwnd);
			else this->CloseWindow(hwnd);
			Sleep(m_tagSetting.iSleepTime);	
			fClose = TRUE;
		}									
	}
	return fClose;
}

/*//////////////////////////////////////////////////////////////////////
// AdjustDialogSize()
// ダイアログ、簡易表示、フル表示きり変え
/*//////////////////////////////////////////////////////////////////////
void
C_CloseExp::AdjustDialogSize(HWND hwnd,BOOL fFullSize){
	RECT rt;
	POINT pt;
	
	/*現在の状態を保持*/
	m_fFullSizeTemp=fFullSize;

	/*スクリーン座標変換*/
	::GetWindowRect(::GetDlgItem(hwnd,IDC_STATIC_MW),&rt);
	pt.x = rt.left;pt.y=rt.top;ScreenToClient(hwnd,&pt);
	rt.left = pt.x;rt.top =pt.y;

	if(fFullSize){	//大きいサイズ

		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),SW_HIDE);

		::MoveWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),rt.left,rt.top,
			SIZE_WIDTH_WAKU,SIZE_HEIGHT_FULL,FALSE);
		::SetWindowPos(::GetDlgItem(hwnd,IDC_CHECK4),NULL,
			MAIN_DLG_KEEP1ST_LEFT,
			MAIN_DLG_HEIGHT_FULL-MAIN_DLG_KEEP1ST_SH,
			0,0, SWP_NOSIZE|SWP_NOZORDER);
		::SetWindowPos(::GetDlgItem(hwnd,IDC_STATIC_PUSH),NULL,
			MAIN_DLG_PULL_LEFT,
			MAIN_DLG_HEIGHT_SMALL-MAIN_DLG_KEEP1ST_SH,
			0,0, SWP_NOSIZE|SWP_NOZORDER);
		mc_maindlg.SizeWindowByClient(MAIN_DLG_WIDTH,MAIN_DLG_HEIGHT_FULL);

		::ShowWindow(::GetDlgItem(hwnd,IDC_BUTTON_EDIT),SW_SHOW);
		for(int i=0;i<MAX_APPEND_ITEMS;i++){
			::ShowWindow(::GetDlgItem(hwnd,k_DlgItemCheck[i]),SW_SHOW);
			::ShowWindow(::GetDlgItem(hwnd,k_DlgItemWinNum[i]),SW_SHOW);
		}
		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_PUSH),SW_SHOW);
		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_PULL),SW_HIDE);

		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),SW_SHOW);


	}else{	//小さいサイズ
		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),SW_HIDE);
		for(int i=0;i<MAX_APPEND_ITEMS;i++){
			::ShowWindow(::GetDlgItem(hwnd,k_DlgItemCheck[i]),SW_HIDE);
			::ShowWindow(::GetDlgItem(hwnd,k_DlgItemWinNum[i]),SW_HIDE);
		}
		::ShowWindow(::GetDlgItem(hwnd,IDC_BUTTON_EDIT),SW_HIDE);
		::MoveWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),rt.left,rt.top,
			SIZE_WIDTH_WAKU,SIZE_HEIGHT_SMALL,FALSE);
		::SetWindowPos(::GetDlgItem(hwnd,IDC_CHECK4),NULL,
			MAIN_DLG_KEEP1ST_LEFT,
			MAIN_DLG_HEIGHT_SMALL-MAIN_DLG_KEEP1ST_SH,
			0,0, SWP_NOSIZE|SWP_NOZORDER);
		::SetWindowPos(::GetDlgItem(hwnd,IDC_STATIC_PULL),NULL,
			MAIN_DLG_PULL_LEFT,
			MAIN_DLG_HEIGHT_SMALL-MAIN_DLG_KEEP1ST_SH,
			0,0, SWP_NOSIZE|SWP_NOZORDER);
		mc_maindlg.SizeWindowByClient(MAIN_DLG_WIDTH,MAIN_DLG_HEIGHT_SMALL);


		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_PULL),SW_SHOW);
		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_PUSH),SW_HIDE);
		::ShowWindow(::GetDlgItem(hwnd,IDC_STATIC_MW),SW_SHOW);
	}
}


/*//////////////////////////////////////////////////////////////////////
// OnMainDlgInit()
// メインダイアログ生成時に呼び出される
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::OnMainDlgInit(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP)
{
	HICON hIcon;
//	char szBuf[128];
	RECT rt;
	hIcon=(HICON)::LoadImage(m_hHinst,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,0);
	::SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
	hIcon=::LoadIcon(m_hHinst,MAKEINTRESOURCE(IDI_ICON1));
	::SendMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);

	//バージョン表示
//	::wsprintf(szBuf,"v%d.%d%s",VERSION_CLEXP_MJ,VERSION_CLEXP_MM,VERSION_CLEXP_CH);
	::SetDlgItemText(hwnd,IDC_STATIC_VER,"");

	HFONT hFont=(HFONT)::SendMessage(::GetDlgItem(hwnd,IDC_STATIC_VER),WM_GETFONT,0,0);
	cCLink.SetFont(hFont);
	::SendDlgItemMessage(hwnd,IDC_CUSTOM_VERSION,WM_HLINKALING_RIGHT,0,0);
	::SendDlgItemMessage(hwnd,IDC_CUSTOM_VERSION,WM_HLINKALING_TARGET,(WPARAM)hwnd,IDC_CUSTOM_VERSION);

	//ダイアログ位置調整
	::GetWindowRect(::GetDesktopWindow(),&rt);
	if(m_tagSetting.iWinLeft >= 0 && m_tagSetting.iWinTop >=0
		&&m_tagSetting.iWinLeft < rt.right && m_tagSetting.iWinTop < rt.bottom){
		::MoveWindow(hwnd,m_tagSetting.iWinLeft,m_tagSetting.iWinTop,10,10,FALSE);
	}

	//ダイアログサイズ調整
	AdjustDialogSize(hwnd,m_tagSetting.fFullSize);

	//ﾁｪｯｸ状態の復元
	if(m_tagSetting.tagTarget.Explorer) mc_maindlg.SetCheckItem(IDC_CHECK1,TRUE);
	if(m_tagSetting.tagTarget.EdgeChrome) mc_maindlg.SetCheckItem(IDC_CHECK2,TRUE);
	if(m_tagSetting.tagTarget.Firefox) mc_maindlg.SetCheckItem(IDC_CHECK3,TRUE);
	int i;
	for(i=0;i<MAX_APPEND_ITEMS;i++){
			if(m_tagSetting.tagTarget.APEND[i].fTarget)
				mc_maindlg.SetCheckItem(k_DlgItemCheck[i],TRUE);
	}
	if(m_tagSetting.fKepp1St) mc_maindlg.SetCheckItem(IDC_CHECK4,TRUE);

	//追加項目の名前文字列設定
	LoadAllTarget();
	for(i=0;i<MAX_APPEND_ITEMS;i++){
		::SetDlgItemText(hwnd,k_DlgItemCheck[i],m_Target.APEND[i].szName);
	}

	//TOPMOSTﾁｪｯｸ
	if(m_tagSetting.fTopmost) {
		mc_maindlg.SetCheckItem(IDC_CHECK5,TRUE);
	}
	OnSetTopmost(hwnd);


	/*タイマーをセットする*/
	::SetTimer(hwnd,MAINDLG_TIMER_ID,MAINDLG_TIMER_TIME,NULL);
	::SetTimer(hwnd,MAINDLG_TIMER_ID_C,MAINDLG_TIMER_TIME_C,NULL);

	/*ｶｳﾝﾄ1回目を実行*/
	CountWIndowNum(hwnd);

	/*手前に出す*/
	::SetWindowPos(hwnd, HWND_TOP ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	::SetForegroundWindow(hwnd);

	return FALSE;
}


/*//////////////////////////////////////////////////////////////////////
// OnHotkey()
// ホットキーが押されたとき
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::OnHotkey(int iKey){
	CLOSE_TARGET_WINDOW tagTarget;
	::ZeroMemory(&tagTarget,sizeof(tagTarget));
	BOOL fMini = FALSE;
	switch(iKey){
	case HOTKEY_ID_SHOWMAIN:
		this->ShowMainDialog();
		return TRUE;
	case HOTKEY_ID_CLOSEEX:
		tagTarget.Explorer = TRUE;
		break;
	case HOTKEY_ID_CLOSEIE:
		tagTarget.EdgeChrome = TRUE;
		break;
	case HOTKEY_ID_CLOSENN:
		tagTarget.Firefox = TRUE;
		break;
	case HOTKEY_ID_CLS_BYSET:
		tagTarget = m_tagSetting.tagTarget;
		break;
	case HOTKEY_ID_MIN_BYSET:
		tagTarget = m_tagSetting.tagTarget;
		fMini = TRUE;
		break;
	default:
		return FALSE;
	}
	CloseExpAllWIndow(FALSE,m_tagSetting.fKepp1St,fMini,&tagTarget);
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// OnMainDlgEnd()
// メインダイアログ終了時に呼び出される
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::OnMainDlgEnd(HWND hwnd,BOOL fRun)
{
	char szBuf[200];

	/*タイマーを削除する*/
	::KillTimer(hwnd,MAINDLG_TIMER_ID);
	::KillTimer(hwnd,MAINDLG_TIMER_ID_C);

	/*キー状態取得*/
	BOOL fContrlOn = (GetAsyncKeyState(VK_CONTROL) &0x8000) ? TRUE : FALSE;

	

	if(fRun || fContrlOn){

		/*チェックボックス状態を得る*/
		m_tagSetting.tagTarget.Explorer = mc_maindlg.IsCheckedItem(IDC_CHECK1);
		m_tagSetting.tagTarget.EdgeChrome = mc_maindlg.IsCheckedItem(IDC_CHECK2);
		m_tagSetting.tagTarget.Firefox = mc_maindlg.IsCheckedItem(IDC_CHECK3);
	//	if(m_fFullSizeTemp){
	//		/*拡張表示になっているときのみ*/
			for(int i=0;i<MAX_APPEND_ITEMS;i++){
				m_tagSetting.tagTarget.APEND[i].fTarget = mc_maindlg.IsCheckedItem(k_DlgItemCheck[i]);
			}
	//	}
		m_tagSetting.fKepp1St = mc_maindlg.IsCheckedItem(IDC_CHECK4);
		BOOL fMinimize = FALSE;
		
		fMinimize = fContrlOn;
		if(fRun){
			::LoadString(mc_maindlg.GetInst(),IDS_STRING_RUNSTR,szBuf,sizeof(szBuf));
			::SetWindowText(hwnd,szBuf);
			CloseExpAllWIndow(FALSE,m_tagSetting.fKepp1St,
				fMinimize,&m_tagSetting.tagTarget);
		}
		/*ウインドウ状態を保存*/
		m_tagSetting.fFullSize = m_fFullSizeTemp;
	}
	/*ウインドウ位置保存*/
	RECT rt;
	::GetWindowRect(hwnd,&rt);
	m_tagSetting.iWinLeft = rt.left;
	m_tagSetting.iWinTop = rt.top;

	/*設定の保存*/
	SaveSettings();

	return FALSE;
}
/*//////////////////////////////////////////////////////////////////////
// OnSetTopmost()
// トップモスト 設定、解除の適用
/*//////////////////////////////////////////////////////////////////////
void
C_CloseExp::OnSetTopmost(HWND hwnd){
	HICON hIcon;
	if(m_tagSetting.fTopmost = mc_maindlg.IsCheckedItem(IDC_CHECK5)){
		mc_maindlg.SetTopMost(TRUE);
		hIcon=::LoadIcon(m_hHinst,MAKEINTRESOURCE(IDI_ICON3));
		::SendDlgItemMessage(hwnd,IDC_CHECK5,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hIcon);
	}else{
		mc_maindlg.SetTopMost(FALSE);
		hIcon=::LoadIcon(m_hHinst,MAKEINTRESOURCE(IDI_ICON2));
		::SendDlgItemMessage(hwnd,IDC_CHECK5,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hIcon);
	}
}

/*//////////////////////////////////////////////////////////////////////
// CountWIndowNum()
// ウインドウの個数を数えてダイアログ内の値を更新
/*//////////////////////////////////////////////////////////////////////
int
C_CloseExp::CountWIndowNum(HWND hwnd)
{
	CLOSE_TARGET_WINDOW tagTarget = m_Target;
	tagTarget.Explorer = tagTarget.EdgeChrome = tagTarget.Firefox = TRUE;
	
	CloseExpAllWIndow(TRUE,FALSE,FALSE,&tagTarget);

	if(m_iNumOfEX)::SetDlgItemInt(hwnd,IDC_STATIC_NEX,m_iNumOfEX,FALSE);
	else ::SetDlgItemText(hwnd,IDC_STATIC_NEX,STR_NONE);
	if(m_iNumOfIE)::SetDlgItemInt(hwnd,IDC_STATIC_NIE,m_iNumOfIE,FALSE);
	else ::SetDlgItemText(hwnd,IDC_STATIC_NIE,STR_NONE);
	if(m_iNumOfNN)::SetDlgItemInt(hwnd,IDC_STATIC_NNN,m_iNumOfNN,FALSE);
	else ::SetDlgItemText(hwnd,IDC_STATIC_NNN,STR_NONE);
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		if(m_aiNumOfAPPEND[i])::SetDlgItemInt(hwnd,k_DlgItemWinNum[i],m_aiNumOfAPPEND[i],FALSE);
		else ::SetDlgItemText(hwnd,k_DlgItemWinNum[i],STR_NONE);
	}
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// OnAboutCloseExp()
// アバウト
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnAboutCloseExp(HWND hwnd)
{
	C_ABOUTDLG c_about;
	c_about.SetCloseExpClassAddress((void*)this);
	if(c_about.OpenRes(m_hHinst,IDD_DIALOG2,hwnd))
		return TRUE; //スタートアップに登録ボタンを押したのでメインダイアログ閉じる
	else  return FALSE;

}

/*//////////////////////////////////////////////////////////////////////
// OnSetting()
// 設定開く
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnSetting(HWND hwnd)
{
	C_SETTINGDLG c_setting;
	c_setting.SetCloseExpClassAddress((void*)this);
	c_setting.OpenRes(m_hHinst,IDD_DIALOGSETTING,hwnd);
	/*設定の保存*/
	SaveSettings();
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// TasktrayAdd()
// タスクトレイ登録
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::TasktrayAdd()
{
	NOTIFYICONDATA	nid;
	HICON		hicon;

	hicon = ::LoadIcon(GetModuleHandle(NULL), (LPSTR)IDI_ICON1);
	if (hicon == 0) return FALSE;
	nid.cbSize	= sizeof(NOTIFYICONDATA);
	nid.hWnd	= mc_dammywindow.GetHandle();
	nid.uID		= 1;
	nid.uFlags	= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.hIcon	= hicon;
	nid.uCallbackMessage = TRAY_CALLBACK;
	::wsprintf(nid.szTip, "CloseExpTray");
	::Shell_NotifyIcon(NIM_ADD, &nid);
	::DestroyIcon(hicon);
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// TasktrayDel()
// タスクトレイから削除
/*//////////////////////////////////////////////////////////////////////
void 
C_CloseExp::TasktrayDel()
{
	NOTIFYICONDATA	nid;

	nid.cbSize	= sizeof(NOTIFYICONDATA);
	nid.hWnd	= mc_dammywindow.GetHandle();
	nid.uID		= 1;

	::Shell_NotifyIcon(NIM_DELETE, &nid);
}



/*//////////////////////////////////////////////////////////////////////
// EnumWindowsProc()  
// ウインドウ列挙、コールバック関数からの呼び出し
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::EnumWindowsProc(HWND hwnd)
{
	char szClassName[256];
	char szWindowName[256];
	::GetClassName(hwnd,szClassName,sizeof(szClassName));
	::GetWindowText(hwnd,szWindowName,sizeof(szWindowName));
	BOOL fClosed=FALSE;

	/*エクスプローラウインドウチェック*/
	if(this->IsEX(hwnd,szClassName,szWindowName)){
		m_iNumOfEX++;
		if(m_Target.Explorer) fClosed = CloseTargetWindows(hwnd);
	}

	/*IEチェック*/
	if(this->IsEdgeOrChrome(hwnd,szClassName,szWindowName)){
		m_iNumOfIE++;
		if(m_Target.EdgeChrome) fClosed = CloseTargetWindows(hwnd);
	}

	/*NNチェック*/
	if(this->IsFirefox(hwnd,szClassName,szWindowName)){
		m_iNumOfNN++;
		if(m_Target.Firefox) fClosed = CloseTargetWindows(hwnd);
	}

	/*拡張ターゲットチェック*/
	for(int i=0;i<MAX_APPEND_ITEMS;i++){

		if(this->IsTarget(hwnd,szClassName,szWindowName,i)){
			m_aiNumOfAPPEND[i] ++;
			if(m_Target.APEND[i].fTarget)  fClosed = CloseTargetWindows(hwnd);
		}
	}
	return TRUE;
}


/*//////////////////////////////////////////////////////////////////////
// CloseExpAllWIndow()  
// 全てのウインドウを閉じる（フラグに基づく）
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::CloseExpAllWIndow(
		BOOL blCountOnly,
		BOOL blKeep1st,
		BOOL blMinisize,
		pCLOSE_TARGET_WINDOW pTarget
		)
{
	m_bCountOnly = blCountOnly;
	m_bKeep1st = blKeep1st;
	m_bMinisize = blMinisize;
	m_Target = *pTarget;
	m_iNumOfEX = m_iNumOfIE = m_iNumOfNN = 0;

	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		m_aiNumOfAPPEND[i] = 0;
		if(!m_fFullSizeTemp){
			m_Target.APEND[i].fTarget = FALSE;
		}
	}
	

	return ::EnumWindows((WNDENUMPROC)EnumWindowsProcDammy,(LPARAM)this);
}

/*//////////////////////////////////////////////////////////////////////
// IsEX()  
// エクスプローラであるかどうかのﾁｪｯｸ
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsEX(HWND hwnd,const LPSTR szClassName,const LPSTR szWindowName)
{
	if(::strcoll("CabinetWClass",szClassName)==0){
		return TRUE;
	}else{
		return FALSE;
	}
}

/*//////////////////////////////////////////////////////////////////////
// IsEX()  
// IEであるかどうかのﾁｪｯｸ
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsFirefox(HWND hwnd,const LPSTR szClassName,const LPSTR szWindowName)
{

	/* 旧バージョン
	if (szClassName[0] == 'A'
		&& szClassName[1] == 'f'
		&& szClassName[2] == 'x'
		&& szClassName[3] == ':') {
		//NN4.xは*Afx:で始まる
		if (::strstr(szWindowName, "Netscape")) {
			//"Netscape"をウインドウテキストに含む
			//以下、Netscapeメッセンジャーとの区別
			HWND hChild;
			char szChildClass[128];
			BOOL fRet = TRUE;
			hChild = ::GetWindow(hwnd, GW_CHILD);
			while (hChild) {
				::GetClassName(hChild, szChildClass, sizeof(szChildClass));
				if (::strcoll("AfxFrameOrView42", szChildClass) == 0) {
					fRet = FALSE;
					break;
				}
				hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
			}
			return fRet;
		}
	}else if (::strcoll("MozillaWindowClass", szClassName) == 0) {
		//6.0は"MozillaWindowClass"をクラス名に持つ
		if (::GetWindowLong(hwnd, GWL_STYLE) == 0x14CF0000) {
			//メニューなども同じクラス名であるので、スタイルをチェック
			return TRUE;//NN6である
		}
	}
	*/

	//firefox 100
	if (::strcoll("MozillaUIWindowClass", szClassName) == 0
		|| ::strcoll("MozillaWindowClass", szClassName) == 0) {
		if (strstr(szWindowName, "Firefox")) {
			return TRUE;
		}
	}
	return FALSE;
}

/*//////////////////////////////////////////////////////////////////////
// IsNN()  
// NNであるかどうかのﾁｪｯｸ
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsEdgeOrChrome(HWND hwnd, const LPSTR szClassName, const LPSTR szWindowName)
{

	if (::strcoll("Chrome_WidgetWin_1", szClassName) == 0) {
		if (::strcoll("", szWindowName) == 0) { //Edgeの隠しウインドウ
			return FALSE;
		}
		if (::strcoll("Tray Assistant", szWindowName) == 0) { //GoogleChromeの常駐トレイ
			return FALSE;
		}

		DWORD dwProcessId;
		GetWindowThreadProcessId(hwnd, &dwProcessId);
		char lpszFileName[4096] = "";

		const HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
		if (hProcess == NULL)
		{
			return FALSE;
		}
		const DWORD length = GetModuleFileNameExA(hProcess, NULL, lpszFileName, sizeof(lpszFileName));
		CloseHandle(hProcess);


		if (strcmp_behind("chrome.exe", lpszFileName) == 0
			|| strcmp_behind("msedge.exe", lpszFileName) == 0)
		{

			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}
/*//////////////////////////////////////////////////////////////////////
// IsTarget()  
// ターゲットであるかどうかのﾁｪｯｸ
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsTarget(HWND hwnd,const LPSTR szClassName,const LPSTR szWindowName,int iTargetNo)
{
	BOOL fRet = FALSE;
	BOOL fClassMatch = FALSE;
	BOOL fTitleMatch = FALSE;
	BOOL fAllWindow = FALSE;
	if(hwnd == mc_maindlg.GetHandle()) return FALSE;

	if(strcoll(m_Target.APEND[iTargetNo].szClass,STR_NONE)==0) fClassMatch = TRUE;
	if(strcoll(m_Target.APEND[iTargetNo].szTitle,STR_NONE)==0) fTitleMatch = TRUE;
	if(strcoll(m_Target.APEND[iTargetNo].szSpec,STR_NONE)==0) fAllWindow = TRUE;
	if(fClassMatch && fTitleMatch && fAllWindow) return FALSE;

	if(strcoll(m_Target.APEND[iTargetNo].szSpec,"%allwindow")==0){
		//全てのウインドウを閉じる
		if(::IsWindowVisible(hwnd)&&::IsWindowEnabled(hwnd)){
			DWORD dwStyle = ::GetWindowLong(hwnd,GWL_STYLE);
			RECT rt;
			/*サイズが０のウインドウは無視する*/
			::GetWindowRect(hwnd,&rt);
			if(rt.left - rt.right!=0 && rt.top - rt.bottom !=0) {
				if(((dwStyle & WS_SYSMENU) == WS_SYSMENU)
					&& ((dwStyle & WS_BORDER) == WS_BORDER)){ 
					fAllWindow = TRUE;
				
				}
			}
		}
	}


	if(strcoll(m_Target.APEND[iTargetNo].szClass,szClassName)==0){
		fClassMatch = TRUE;
	}
//	fTitleMatch = TRUE;fClassMatch = TRUE;
	if(m_Target.APEND[iTargetNo].szTitle[0]=='!'){
		//前方不一致
		if(!strstr(szWindowName,&(m_Target.APEND[iTargetNo].szTitle[1]))){
			fTitleMatch = TRUE;
		}
	}else{
		//前方一致
		if(strstr(szWindowName,m_Target.APEND[iTargetNo].szTitle)){
			fTitleMatch = TRUE;
		}
	}
	fRet = (fClassMatch && fTitleMatch && fAllWindow);
	
	return fRet;
}
/*//////////////////////////////////////////////////////////////////////
// CloseWindow()  
// 引数のウインドウを閉じる
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::CloseWindow(HWND hwnd)
{
	::PostMessage(hwnd,WM_SYSCOMMAND,SC_CLOSE,0);
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// CloseWindow()  
// 引数のウインドウを最小化する
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::MinisizeWindow(HWND hwnd)
{
	::ShowWindow(hwnd,SW_HIDE);
	::ShowWindow(hwnd,SW_MINIMIZE);
	::ShowWindow(hwnd,SW_SHOW);
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// RegisterHotkeys()  
// ホットキーを登録する
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::RegisterHotkeys(){
	HWND hwnd = mc_dammywindow.GetHandle();
	if(m_fRegistHotkey) UnRegisterHotkeys();
	m_fRegistHotkey = TRUE;
	switch(m_tagSetting.iHotKeyMode){
	case MODE_HOTKEY_NORMAL:

		::RegisterHotKey(hwnd, HOTKEY_ID_CLOSEEX, MOD_ALT | MOD_SHIFT | MOD_CONTROL, 69); // 69=[E]
		::RegisterHotKey(hwnd, HOTKEY_ID_CLOSEIE, MOD_ALT | MOD_SHIFT | MOD_CONTROL, 67); // 67=[C]
		::RegisterHotKey(hwnd,HOTKEY_ID_CLOSENN, MOD_ALT | MOD_SHIFT | MOD_CONTROL,70 );//70=[F]
		
		

		::RegisterHotKey(hwnd, HOTKEY_ID_SHOWMAIN, MOD_ALT | MOD_SHIFT| MOD_CONTROL, 87); // 87=[W]
		break;
	case MODE_HOTKEY_NOWIN:
//		::RegisterHotKey(hwnd,HOTKEY_ID_CLOSENN,MOD_ALT|MOD_SHIFT,87 );
//		::RegisterHotKey(hwnd,HOTKEY_ID_CLOSEIE,MOD_ALT|MOD_SHIFT|MOD_CONTROL,87 );
//		::RegisterHotKey(hwnd,HOTKEY_ID_SHOWMAIN,MOD_ALT,87 );
//		::RegisterHotKey(hwnd,HOTKEY_ID_CLOSEEX,MOD_ALT|MOD_CONTROL,87 );
		::RegisterHotKey(hwnd,HOTKEY_ID_SHOWMAIN,MOD_ALT|MOD_SHIFT,87 );
		break;
	}
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// RegisterHotkeys()  
// ホットキーを解除する
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::UnRegisterHotkeys(){
	if(m_fRegistHotkey == FALSE) return FALSE;
	HWND hwnd = mc_dammywindow.GetHandle();
	switch(m_tagSetting.iHotKeyMode){
		case MODE_HOTKEY_NORMAL:
		::UnregisterHotKey(hwnd,HOTKEY_ID_SHOWMAIN);
		::UnregisterHotKey(hwnd,HOTKEY_ID_CLOSEEX);
		::UnregisterHotKey(hwnd,HOTKEY_ID_CLOSEIE);
		::UnregisterHotKey(hwnd,HOTKEY_ID_CLOSENN);
			break;	
		case MODE_HOTKEY_NOWIN:
			::UnregisterHotKey(hwnd,HOTKEY_ID_SHOWMAIN);
			break;
	}
	m_fRegistHotkey = FALSE;
	return TRUE;
}


/*//////////////////////////////////////////////////////////////////////
// OnTaskTrayCallBack()
// タスクトレイコールバック関数
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnTaskTrayCallBack(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP)
{
	HMENU hMenu;
	POINT pt;
	switch (lP) {
		case WM_RBUTTONDOWN:
			::GetCursorPos(&pt);
			hMenu = ::LoadMenu((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE)
						,MAKEINTRESOURCE(IDR_MENU1));
			hMenu = ::GetSubMenu(hMenu, 0);
			if(m_tagSetting.fKepp1St){
				MENUITEMINFO mi;
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState  = m_tagSetting.fKepp1St ?  MFS_CHECKED : NULL ;
				//ID_MENU_KEEP1ST = 一番上は閉じない
				::SetMenuItemInfo(hMenu,ID_MENU_KEEP1ST,FALSE ,&mi);
			}
			::TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON,
					pt.x, pt.y, 0, hwnd, NULL);
		
		
			break;
		case WM_LBUTTONDOWN:
			this->ShowMainDialog();
			break;
	}
	return 0;
}

/*//////////////////////////////////////////////////////////////////////
// OnExitCloseExp()
// 終了時に1度だけ呼び出される
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnExitCloseExp()
{
	/*タスクトレイ除去*/
	this->TasktrayDel();

	/*ホットキー解除*/
	this->UnRegisterHotkeys();

	/*設定の保存*/
	this->SaveSettings();

	::PostQuitMessage(0);
//	mc_dammywindow.SendMessage(WM_DESTROY,0,0);
	return 0;
}



/*//////////////////////////////////////////////////////////////////////
// ShowMainDialog()
// メインダイアログの表示
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::ShowMainDialog(void)
{
	if(m_MainDlgCreated) {//既に表示中
			/*手前に出す*/
		::SetWindowPos(mc_maindlg.GetHandle(), HWND_TOP ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		::SetForegroundWindow(mc_maindlg.GetHandle());
		return FALSE;
	}
	m_MainDlgCreated = TRUE;
	mc_maindlg.OpenRes(m_hHinst,IDD_DIALOG1,NULL);
	m_MainDlgCreated = FALSE;

	/*プロセスサイズを小さくする*/
	::SetProcessWorkingSetSize(GetCurrentProcess(),0xffffffff, 0xffffffff );
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// LoadSettings()
// セッティングの読み込み
/*//////////////////////////////////////////////////////////////////////

inline BOOL isTrueOrFalse(char* szStrBool){
	if(strcoll(szStrBool,STR_TRUE)==0) return TRUE;
	else return FALSE;
};

BOOL
C_CloseExp::LoadSettings(void)
{
	char szFilename[MAX_PATH+50];
	char szGet[128];
	char szStrBuf[128];
	CPath cpath;

	//
	if(cpath.GetSpecialFolderFullPath(szFilename,sizeof(szFilename),CSIDL_APPDATA,"KNO\\CloseExp\\default.ini")){
		if(!IsExistFile(szFilename)){
			cpath.GetFullPath(szFilename,sizeof(szFilename),STR_INIFILE);
		}
	}else{
		cpath.GetFullPath(szFilename,sizeof(szFilename),STR_INIFILE);
	}
	//



	/*[TARGET]*/
	::GetPrivateProfileString("TARGET","EX",STR_TRUE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.tagTarget.Explorer = ::isTrueOrFalse(szGet);
	::GetPrivateProfileString("TARGET","IE",STR_FALSE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.tagTarget.EdgeChrome = ::isTrueOrFalse(szGet);
	::GetPrivateProfileString("TARGET","NN",STR_FALSE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.tagTarget.Firefox = ::isTrueOrFalse(szGet);
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		::wsprintf(szStrBuf,"AP%02d",i);
		::GetPrivateProfileString("TARGET",szStrBuf,STR_FALSE,szGet,sizeof(szGet),szFilename);
		m_tagSetting.tagTarget.APEND[i].fTarget = ::isTrueOrFalse(szGet);
	}
	/*[OPTIONS]*/
	::GetPrivateProfileString("OPTIONS","TOPMOST",STR_TRUE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.fTopmost = ::isTrueOrFalse(szGet);
	::GetPrivateProfileString("OPTIONS","KEEP1ST",STR_FALSE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.fKepp1St = ::isTrueOrFalse(szGet);
	::GetPrivateProfileString("OPTIONS","FULLSIZE",STR_FALSE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.fFullSize = ::isTrueOrFalse(szGet);


	/*[RECT]*/
	::GetPrivateProfileString("RECT","LEFT","-1",szGet,sizeof(szGet),szFilename);
	m_tagSetting.iWinLeft = ::atoi(szGet);
	::GetPrivateProfileString("RECT","TOP","-1",szGet,sizeof(szGet),szFilename);
	m_tagSetting.iWinTop = ::atoi(szGet);

	/*[HOTKEY]*/
	::GetPrivateProfileString("HOTKEY","MODE",MODE_HOTKEY_NORMAL,szGet,sizeof(szGet),szFilename);
	m_tagSetting.iHotKeyMode = atoi(szGet);

	/*[SETTINGS]*/
	::GetPrivateProfileString("SETTINGS","NOBACK",STR_FALSE,szGet,sizeof(szGet),szFilename);
	m_tagSetting.fNotBackground = ::isTrueOrFalse(szGet);
	::GetPrivateProfileString("SETTINGS","WAITTIME",DEFAULT_SLEEP_TIME,szGet,sizeof(szGet),szFilename);
	m_tagSetting.iSleepTime = ::atoi(szGet);
	::GetPrivateProfileString("SETTINGS","RUNNEXT",STR_TRUE,szGet,sizeof(szGet),szFilename);
	return ::isTrueOrFalse(szGet);


}
/*//////////////////////////////////////////////////////////////////////
// SaveSettings()
// セッティングの保存
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::SaveSettings(void)
{
	char szFilename[MAX_PATH+50];
	char szStrset[128];
	char szStrBuf[128];
	CPath cpath;


	//
	if(cpath.GetSpecialFolderFullPath(szFilename,sizeof(szFilename),CSIDL_APPDATA,"KNO")){
		CreateDirectory(szFilename,NULL);
		cpath.GetSpecialFolderFullPath(szFilename,sizeof(szFilename),CSIDL_APPDATA,"KNO\\CloseExp");
		CreateDirectory(szFilename,NULL);
		cpath.GetSpecialFolderFullPath(szFilename,sizeof(szFilename),CSIDL_APPDATA,"KNO\\CloseExp\\default.ini");
	}else{
		cpath.GetFullPath(szFilename,sizeof(szFilename),STR_INIFILE);
	}
	//

//	
	/*[TARGET]*/
	::WritePrivateProfileString(
		"TARGET","EX",
		(m_tagSetting.tagTarget.Explorer ? STR_TRUE : STR_FALSE),
		szFilename);
	::WritePrivateProfileString(
		"TARGET","IE",
		(m_tagSetting.tagTarget.EdgeChrome ? STR_TRUE : STR_FALSE),
		szFilename);
	::WritePrivateProfileString(
		"TARGET","NN",
		(m_tagSetting.tagTarget.Firefox ? STR_TRUE : STR_FALSE),
		szFilename);
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		::wsprintf(szStrBuf,"AP%02d",i);
		::WritePrivateProfileString(
			"TARGET",szStrBuf,
			(m_tagSetting.tagTarget.APEND[i].fTarget ? STR_TRUE : STR_FALSE),
			szFilename);
	}
	/*[OPTIONS]*/
	::WritePrivateProfileString(
		"OPTIONS","TOPMOST",
		(m_tagSetting.fTopmost ? STR_TRUE : STR_FALSE),
		szFilename);
	::WritePrivateProfileString(
		"OPTIONS","KEEP1ST",
		(m_tagSetting.fKepp1St ? STR_TRUE : STR_FALSE),
		szFilename);
	::WritePrivateProfileString(
		"OPTIONS","FULLSIZE",
		(m_tagSetting.fFullSize ? STR_TRUE : STR_FALSE),
		szFilename);

	/*[RECT]*/
	::wsprintf(szStrset,"%d",m_tagSetting.iWinLeft);
	::WritePrivateProfileString("RECT","LEFT",szStrset,szFilename);
	::wsprintf(szStrset,"%d",m_tagSetting.iWinTop);
	::WritePrivateProfileString("RECT","TOP",szStrset,szFilename);

	/*[HOTKEY]*/
	::wsprintf(szStrset,"%d",m_tagSetting.iHotKeyMode);
	::WritePrivateProfileString("HOTKEY","MODE",szStrset,szFilename);

	/*[SETTINGS]*/
	::WritePrivateProfileString(
		"SETTINGS","NOBACK",
		(m_tagSetting.fNotBackground ? STR_TRUE : STR_FALSE),
		szFilename);
	::wsprintf(szStrset,"%d",m_tagSetting.iSleepTime);
	::WritePrivateProfileString("SETTINGS","WAITTIME",szStrset,szFilename);
	::WritePrivateProfileString("SETTINGS","RUNNEXT",STR_FALSE,szFilename);
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// LoadTarget()
// ターゲットファイルを読み込む
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::LoadTarget(int iKey,pCLEXP_APPEND ptagCls_append)
{
	char szFilename[MAX_PATH];
	char szKey[32];
	CPath cpath;
	cpath.GetFullPath(szFilename,sizeof(szFilename),STR_TARGET);

	/*[TARGETxx]*/
	::wsprintf(szKey,"TARGET%02d",iKey);
	::GetPrivateProfileString(szKey,"NAME",STR_NONE,ptagCls_append->szName,sizeof(ptagCls_append->szName),szFilename);
	::GetPrivateProfileString(szKey,"SPEC",STR_NONE,ptagCls_append->szSpec,sizeof(ptagCls_append->szSpec),szFilename);
	::GetPrivateProfileString(szKey,"CLASS",STR_NONE,ptagCls_append->szClass,sizeof(ptagCls_append->szClass),szFilename);
	::GetPrivateProfileString(szKey,"TEXT",STR_NONE,ptagCls_append->szTitle,sizeof(ptagCls_append->szTitle),szFilename);
	
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// LoadAllTarget()
// ターゲットファイル内の全て読み込む
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::LoadAllTarget(){
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		LoadTarget(i,&m_Target.APEND[i]);
		LoadTarget(i,&m_tagSetting.tagTarget.APEND[i]);
	}
	return TRUE;
}

BOOL 
C_CloseExp::OnEditTarget(HWND hwmd)
{
	char szDir[MAX_PATH];
	CPath cPath;
	cPath.GetExecutePath(szDir,sizeof(szDir));
	::ShellExecute(NULL,"open",STR_TARGET,NULL,szDir,SW_SHOWNORMAL);
	return TRUE;
}
/*##############################################################################
  CLASS C_MainDialog
##############################################################################*/

//---------------------------------------------------------------------
// メインダイアログプロージャ関数
//---------------------------------------------------------------------
LRESULT CALLBACK 
C_MainDialog::DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	static BOOL fFirst =TRUE;
	switch (uMsg) {
	case WM_INITDIALOG:

		/*ダイアログ初期化*/
		return c_closeexp->OnMainDlgInit(hwnd,uMsg,wP,lP);
	case WM_TIMER:
		switch((int)wP){
		case MAINDLG_TIMER_ID://現在の数をｶｳﾝﾄ
			c_closeexp->CountWIndowNum(hwnd);break;
		case MAINDLG_TIMER_ID_C:
			if(fFirst){	//1度だけ実行
				fFirst = FALSE;
				pCLEXP_SETTING ptagSet = c_closeexp->GetSettingTag();
				if(ptagSet->fTopmost) {
					::SetForegroundWindow(hwnd);
				}
			}
			ChangeBtnString(hwnd);
			break;
		}
		break;
	case WM_HLINKALING_TARGET:
		switch(lP){
		case IDC_CUSTOM_VERSION:
			if(c_closeexp->OnAboutCloseExp(hwnd)){
				c_closeexp->OnMainDlgEnd(hwnd,FALSE);
				::EndDialog(hwnd,0);
			}
			break;
		}
		break;
	case WM_COMMAND:

		switch (LOWORD(wP)){
		case IDC_CHECK5:	//最上面固定クリップ
			c_closeexp->OnSetTopmost(hwnd);
			break;
		case IDC_STATIC_PULL:
			c_closeexp->AdjustDialogSize(hwnd,TRUE);
			break;
		case IDC_STATIC_PUSH:
			c_closeexp->AdjustDialogSize(hwnd,FALSE);
			break;
		case IDOK:

			/*ダイアログの終了*/
			c_closeexp->OnMainDlgEnd(hwnd,TRUE);
			::EndDialog(hwnd,1);
			break;
		case IDCHHIDE:
		case IDCANCEL:
			/*ダイアログの終了*/
			c_closeexp->OnMainDlgEnd(hwnd,FALSE);
			::EndDialog(hwnd,0);
			break;
//		case IDC_STATIC_VER:
//		case IDC_CUSTOM_VERSION:
//			if(c_closeexp->OnAboutCloseExp(hwnd)){
//				c_closeexp->OnMainDlgEnd(hwnd,FALSE);
//				::EndDialog(hwnd,0);
//			}
//			break;
		case IDC_BUTTON_EDIT:
			c_closeexp->OnEditTarget(hwnd);
			c_closeexp->OnMainDlgEnd(hwnd,FALSE);
			::EndDialog(hwnd,0);
			break;
	
			
		}
		break;
	}
	return 0;
};

//---------------------------------------------------------------------
// コントロールキーの状態に合わせてボタンの文字列変化
//---------------------------------------------------------------------
void C_MainDialog::ChangeBtnString(HWND hwnd){

	char szBuf[32];
	static BOOL fPrevState = FALSE;//１つ前の状態　FALSE=通常

	if(GetAsyncKeyState(VK_CONTROL) &0x8000){
		if(fPrevState){
			::LoadString(this->GetInst(),IDS_STRING_ALLMINI,szBuf,sizeof(szBuf));
			::SetDlgItemText(hwnd,IDOK,szBuf);
			::LoadString(this->GetInst(),IDS_STRING_ONLYSET,szBuf,sizeof(szBuf));
			::SetDlgItemText(hwnd,IDCHHIDE,szBuf);
		}
		fPrevState = TRUE;
	}else{
		if(!fPrevState){
			::LoadString(this->GetInst(),IDS_STRING_CLOSE,szBuf,sizeof(szBuf));
			::SetDlgItemText(hwnd,IDOK,szBuf);
			::LoadString(this->GetInst(),IDS_STRING_CANCEL,szBuf,sizeof(szBuf));
			::SetDlgItemText(hwnd,IDCHHIDE,szBuf);
		}
		fPrevState = FALSE;
	}
}

void 
C_MainDialog::SetCloseExpClassAddress(void* cle)
{
	m_clsexp=cle;
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	m_ptagClsSets = c_closeexp->GetSettingTag();
}



/*##############################################################################
  CLASS C_DammyWindow
##############################################################################*/


/*//////////////////////////////////////////////////////////////////////
// WndProc()
// ダミーウインドウ、プロージャ
/*//////////////////////////////////////////////////////////////////////
LRESULT
C_DammyWindow::WndProc( UINT uMsg, WPARAM wP, LPARAM lP ){
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	switch (uMsg) {
	case WM_DESTROY:
		c_closeexp->OnExitCloseExp();
	//	PostQuitMessage(0);
		break;
	case TRAY_CALLBACK:
		return c_closeexp->OnTaskTrayCallBack(this->GetHandle(),uMsg,wP,lP);
	case WM_CLEXP_SHOWDLG://メインダイアログ表示
		c_closeexp->ShowMainDialog();
		break;
	case WM_HOTKEY://ホットキーがおされたとき
		c_closeexp->OnHotkey((int)wP);
		break;
	case WM_COMMAND:

		switch (LOWORD(wP)){
		case WM_ENDSESSION :
		case ID_MENU_EXIT://終了
			return c_closeexp->OnExitCloseExp();
		case ID_MENU_KEEP1ST://最初のウインドウは閉じない
			c_closeexp->OnMenuKeep1st();
			break;
		case ID_MENU_CLOSEEX://エクスプローラ閉じる
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSEEX,0);
			break;
		case ID_MENU_CLOSEIE://IE閉じる
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSEIE,0);
			break;
		case ID_MENUCLOSENN://NN閉じる
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSENN,0);
			break;
		case ID_MENU_RUN_MIN://現在の設定ですべて最小化
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_MIN_BYSET,0);
			break;
		case ID_MENU_RUN_CLS://現在の設定ですべて閉じる。
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLS_BYSET,0);
			break;
		case ID_MEN_OPEN://メインウインドウを開く
			c_closeexp->ShowMainDialog();
			break;
		case ID_MENU_ABOUT://アバウト
			c_closeexp->OnAboutCloseExp(NULL);
			break;
		case ID_MENUITEMSETTING://設定
			c_closeexp->OnSetting(NULL);
			break;

		}
		break;
	}

	return ::DefWindowProc( this->GetHandle(), uMsg, wP, lP );
};

void 
C_DammyWindow::SetCloseExpClassAddress(void* cle)
{
	m_clsexp=cle;

}

/*##############################################################################
  CLASS C_ABOUTDLG
##############################################################################*/
LRESULT CALLBACK 
C_ABOUTDLG::DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP)
{
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	pCLEXP_SETTING ptagSettingTemp;
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			char szBuf[128];
			ptagSettingTemp = c_closeexp->GetSettingTag();	
			//バージョン表示
			::wsprintf(szBuf,STR_VERSION,VERSION_CLEXP_MJ,VERSION_CLEXP_MM,VERSION_CLEXP_CH,STR_DATE);
			::SetDlgItemText(hwnd,IDC_STATIC_VERSION,szBuf);
			ptagSettingTemp = c_closeexp->GetSettingTag();
			this->SetCheckItem(IDC_CHECK_NOBACK,ptagSettingTemp->fNotBackground);
			if(ptagSettingTemp->iHotKeyMode ==0){
				this->SetCheckItem(IDC_RADIO_HMODE1,TRUE);
			}else{
				this->SetCheckItem(IDC_RADIO_HMODE2,TRUE);
			}
		}
		break;

	case WM_COMMAND:

		switch (LOWORD(wP)){
		case IDOK:
		case IDCANCEL:
			{
				int iHotkeyMode;
				ptagSettingTemp = c_closeexp->GetSettingTag();
				ptagSettingTemp->fNotBackground = this->IsCheckedItem(IDC_CHECK_NOBACK);
				iHotkeyMode = this->IsCheckedItem(IDC_RADIO_HMODE1) ? 0 : 1;
				if(ptagSettingTemp->iHotKeyMode != iHotkeyMode){
					c_closeexp->UnRegisterHotkeys();
					ptagSettingTemp->iHotKeyMode = iHotkeyMode;
					c_closeexp->RegisterHotkeys();
				}
				::EndDialog(hwnd,0);
			}
			break;
		case IDC_REG_STARTMENU:
			{
				char szDir[MAX_PATH];
				CPath cPath;
				cPath.GetExecutePath(szDir,sizeof(szDir));
				::ShellExecute(NULL,"open",STR_FN_LINK,NULL,szDir,SW_SHOWNORMAL);
				::EndDialog(hwnd,1);
			}
			break;
		}
		break;
	}
	return 0;

}

void 
C_ABOUTDLG::SetCloseExpClassAddress(void* cle)
{
	m_clsexp=cle;

}


/*##############################################################################
  CLASS C_ABOUTDLG
##############################################################################*/
LRESULT CALLBACK 
C_SETTINGDLG::DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP)
{
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	pCLEXP_SETTING ptagSettingTemp;
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			ptagSettingTemp = c_closeexp->GetSettingTag();
			this->SetCheckItem(IDC_CHECK_NOBACK,ptagSettingTemp->fNotBackground);
			::SetDlgItemInt(hwnd,IDC_EDIT_WAITTIME,ptagSettingTemp->iSleepTime,FALSE);
		}
		break;

	case WM_COMMAND:

		switch (LOWORD(wP)){
		case IDOK:
			ptagSettingTemp = c_closeexp->GetSettingTag();
			ptagSettingTemp->fNotBackground = this->IsCheckedItem(IDC_CHECK_NOBACK);
			ptagSettingTemp->iSleepTime = ::GetDlgItemInt(hwnd,IDC_EDIT_WAITTIME,NULL,FALSE);
			if(ptagSettingTemp->iSleepTime > MAX_SLEEP_TIME){ 
				::SetDlgItemInt(hwnd,IDC_EDIT_WAITTIME,MAX_SLEEP_TIME,FALSE);
				break;
			}
			::EndDialog(hwnd,0);
			break;
		case IDCANCEL:
			::EndDialog(hwnd,0);
			break;
		}
		break;
	}
	return 0;

}

void 
C_SETTINGDLG::SetCloseExpClassAddress(void* cle)
{
	m_clsexp=cle;

}