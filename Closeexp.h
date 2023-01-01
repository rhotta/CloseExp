////////////////////////////////////////////////////////////////////////
//
//FileName: closeexp.h
//アプリケーション: CloseEXP
//
//制作日 01/--/--
//
//(c)2001 RyusukeHotta

//-----------------------------------
// define
//-----------------------------------

#define MAX_STR				512			//最大文字列数（ウインドウ）

#define TRAY_CALLBACK		WM_USER +10	//タスクトレイコールバックID
#define WM_CLEXP_SHOWDLG	WM_USER +20 //メインダイアログ表示（2重起動用）

#define HOTKEY_ID_SHOWMAIN	1			//ホットキーID（メインウインドウ表示）
#define HOTKEY_ID_CLOSEEX	2
#define HOTKEY_ID_CLOSEIE	3			// IE
#define HOTKEY_ID_CLOSENN	4
#define HOTKEY_ID_CLS_BYSET	5			// 現在の設定で全て閉じる
#define HOTKEY_ID_MIN_BYSET	6			// 現在の設定ですべて最小化

#define MAINDLG_TIMER_ID	300			//タイマーID
#define MAINDLG_TIMER_TIME	2000		//タイマー間隔
#define MAINDLG_TIMER_ID_C	301			//タイマーID (Ctrl検出)
#define MAINDLG_TIMER_TIME_C 100		//タイマー間隔(Ctrl検出)

#define SIZE_WIDTH_WAKU		195			//メインダイアログ枠幅
#define SIZE_HEIGHT_SMALL	70			//メインダイアログ枠高さ （縮小時）
#define SIZE_HEIGHT_FULL	150			//メインダイアログ枠高さ （拡大時）

#define MAIN_DLG_WIDTH		312			//メインダイアログ幅

#define MAIN_DLG_HEIGHT_SMALL	105		//メインダイアログ高さ（縮小時）
#define MAIN_DLG_HEIGHT_FULL	185		//メインダイアログ高さ（拡大時）

#define MAIN_DLG_KEEP1ST_SH		21		//１こめは閉じない ウインドウの下からの相対位置
#define MAIN_DLG_KEEP1ST_LEFT	12		//１こめは閉じない 左からの位置

#define MAIN_DLG_PULL_LEFT		288		//詳細表示ﾁｪｯｸボックス左からの位置

#define DEFAULT_SLEEP_TIME		"50"	//１個閉じたあとの待機時間文字列（ミリ秒）
#define MAX_SLEEP_TIME			200		//1個閉じたあとの待ち時間設定できる最大値

#define MAX_APPEND_ITEMS	5			//追加アイテム数は5個

#define VERSION_CLEXP_MJ	2			//バージョン整数
#define VERSION_CLEXP_MM	7
#define VERSION_CLEXP_CH	""			//バージョン付加文字列

#define MODE_HOTKEY_NORMAL	0			//ホットキーモード
#define MODE_HOTKEY_NOWIN	1			//

#define STR_CLEXP_MUTEX		"CLOSE_EXP"		//MUTEX
#define STR_CLEXP_CLASS		"CloseExpDummy"	//CLass
#define STR_CLEXP_WINTEXT	"CLSEXPDMMY"	//WindowText

#define STR_DATE			"2022/12/30"//バージョン文字列
#define STR_VERSION			"CloseExp  Version %d.%d%s   (c)2001-2022 R.HOTTA   %s"

//#define STR_CLOSING			"実行中..."

#define STR_INIFILE			"clexp.ini"	//設定保存ファイル
#define STR_TARGET			"target.ini"
#define STR_FN_LINK			"AutoRunRegister.exe"

#define STR_FALSE			"false"
#define STR_TRUE			"true"

#define STR_NONE			"-"
#define STR_ZERO			"0"
//-----------------------------------
// const
//-----------------------------------
static const int k_DlgItemCheck[MAX_APPEND_ITEMS] ={
					IDC_CHECK6,
					IDC_CHECK7,
					IDC_CHECK8,
					IDC_CHECK9,
					IDC_CHECK10,
				};
static const int k_DlgItemWinNum[MAX_APPEND_ITEMS] ={
					IDC_STATIC_NNN2,
					IDC_STATIC_NNN3,
					IDC_STATIC_NNN4,
					IDC_STATIC_NNN5,
					IDC_STATIC_NNN6,
				};
//-----------------------------------
// typedef
//-----------------------------------

//拡張ターゲット
typedef struct{
	char szName[128];	//設定名
	char szSpec[32];	//特殊な条件
	char szClass[128];	//クラス名
	char szTitle[256];	//ウインドウタイトル
	BOOL fTarget;		//操作対象か（ﾁｪｯｸされているか）
}CLEXP_APPEND, *pCLEXP_APPEND;

//ターゲット
typedef struct{
	BOOL Explorer;
	BOOL EdgeChrome;
	BOOL Firefox;
	CLEXP_APPEND APEND[MAX_APPEND_ITEMS];
}CLOSE_TARGET_WINDOW,*pCLOSE_TARGET_WINDOW;

//保存する設定
typedef struct {
	BOOL fTopmost;

	CLOSE_TARGET_WINDOW tagTarget;
	BOOL fKepp1St;

	BOOL fFullSize;

	int iSleepTime;//ウインドウ１個の待ち時間

	int iWinLeft;//座標、負の値のときは使用されない
	int iWinTop;

	int iHotKeyMode;

	BOOL fNotBackground;	//非常駐モード
}CLEXP_SETTING, *pCLEXP_SETTING;



/*######################################################################
//  class C_MainDialog
// メインダイアログ
######################################################################*/
class C_MainDialog:public CMDialog{
public:
	LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);
	void ChangeBtnString(HWND hwnd);
	void SetCloseExpClassAddress(void* cle);
private:
	void* m_clsexp;	
	pCLEXP_SETTING m_ptagClsSets;
};

/*######################################################################
//  class C_MainDialog
// ダミーウインドウ
######################################################################*/
class C_DammyWindow:public Cwindow{
public:
	LRESULT WndProc( UINT uMsg, WPARAM wP, LPARAM lP );
	void SetCloseExpClassAddress(void* cle);
private:
	void* m_clsexp;

};
/*######################################################################
//  class C_ABOUTDLG
// アバウト表示ダイアログ
######################################################################*/
class C_ABOUTDLG:public CMDialog{
public:
	void SetCloseExpClassAddress(void* cle);
	LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);
private:
	void* m_clsexp;
};
/*######################################################################
//  class C_SETTINGDLG
// 設定ダイアログ
######################################################################*/
class C_SETTINGDLG:public CMDialog{
public:
	void SetCloseExpClassAddress(void* cle);
	LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);
private:
	void* m_clsexp;
};
/*######################################################################
//  class C_CloseExp
// メインクラス
######################################################################*/
//メンバ関数の詳細はcpp

class C_CloseExp{

public: 
	C_CloseExp();


	int StartUp(HINSTANCE hThisInst,LPSTR lpszArgs);

	void AdjustDialogSize(HWND hwnd,BOOL fFullSize);

	BOOL CloseTargetWindows(HWND hwnd);

	BOOL OnMainDlgInit(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);
	BOOL OnMainDlgEnd(HWND hwnd,BOOL fRun);

	BOOL TasktrayAdd();
	void TasktrayDel();

	BOOL EnumWindowsProc(HWND hwnd);

	BOOL CloseExpAllWIndow(
		BOOL blCountOnly,
		BOOL blKeep1st,
		BOOL blMinisize,
		pCLOSE_TARGET_WINDOW pTarget
		);

	BOOL IsEX(HWND hwnd, const LPSTR szClassName, const LPSTR szWindowName);

	BOOL IsFirefox(HWND hwnd, const LPSTR szClassName, const LPSTR szWindowName);

	BOOL IsEdgeOrChrome(HWND hwnd,const LPSTR szClassName,const LPSTR szWindowName);
	
	
	BOOL IsTarget(HWND hwnd,const LPSTR szClassName,
		const LPSTR szWindowName,int iTargetNo);

//	LRESULT CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);

	BOOL OnTaskTrayCallBack(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP);
	BOOL OnExitCloseExp();

	BOOL OnHotkey(int iKey);

	BOOL ShowMainDialog(void);

	int CountWIndowNum(HWND hwnd);

	void OnMenuKeep1st(){m_tagSetting.fKepp1St=!m_tagSetting.fKepp1St;};

	BOOL OnAboutCloseExp(HWND hwnd);
	BOOL OnSetting(HWND hwnd);

	void OnSetTopmost(HWND hwnd);
	pCLEXP_SETTING GetSettingTag(){return &m_tagSetting;};

	BOOL LoadSettings();//設定読み込み
	BOOL SaveSettings();

	BOOL LoadTarget(int iKey,pCLEXP_APPEND ptagCls_append);
	BOOL LoadAllTarget();

	BOOL OnEditTarget(HWND hwnd);

	BOOL RegisterHotkeys();
	BOOL UnRegisterHotkeys();
protected:
	BOOL CloseWindow(HWND hwnd);
	BOOL MinisizeWindow(HWND hwnd);
private:
	HINSTANCE m_hHinst;

	BOOL m_MainDlgCreated;	//すでに表示している
	BOOL m_fRegistHotkey;	//ホットキーを設定済み
//	int m_iHotKeyMode;

	BOOL m_bKeep1st;		//最初のウインドウは無視(実行するとFALSEに)
	BOOL m_bCountOnly;		//ｶｳﾝﾄのみする場合
	BOOL m_bMinisize;		//最小化する場合

	CLOSE_TARGET_WINDOW m_Target;	//ターゲット

	int m_iNumOfEX;			//それぞれの数
	int m_iNumOfIE;
	int m_iNumOfNN;
	int m_aiNumOfAPPEND[MAX_APPEND_ITEMS];

	BOOL m_fFullSizeTemp=FALSE;	//表示サイズ
							//（一時的、ウインドウがある間だけ）＝現在の状態

	C_MainDialog	mc_maindlg;		//メインDIALOG
	C_DammyWindow	mc_dammywindow;	//ウインドウ

	/*以下、保存必要な設定*/
	CLEXP_SETTING m_tagSetting;

	C_CTRL_HYPERLINK cCLink;

};




