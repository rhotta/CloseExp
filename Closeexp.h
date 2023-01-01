////////////////////////////////////////////////////////////////////////
//
//FileName: closeexp.h
//�A�v���P�[�V����: CloseEXP
//
//����� 01/--/--
//
//(c)2001 RyusukeHotta

//-----------------------------------
// define
//-----------------------------------

#define MAX_STR				512			//�ő啶���񐔁i�E�C���h�E�j

#define TRAY_CALLBACK		WM_USER +10	//�^�X�N�g���C�R�[���o�b�NID
#define WM_CLEXP_SHOWDLG	WM_USER +20 //���C���_�C�A���O�\���i2�d�N���p�j

#define HOTKEY_ID_SHOWMAIN	1			//�z�b�g�L�[ID�i���C���E�C���h�E�\���j
#define HOTKEY_ID_CLOSEEX	2
#define HOTKEY_ID_CLOSEIE	3			// IE
#define HOTKEY_ID_CLOSENN	4
#define HOTKEY_ID_CLS_BYSET	5			// ���݂̐ݒ�őS�ĕ���
#define HOTKEY_ID_MIN_BYSET	6			// ���݂̐ݒ�ł��ׂčŏ���

#define MAINDLG_TIMER_ID	300			//�^�C�}�[ID
#define MAINDLG_TIMER_TIME	2000		//�^�C�}�[�Ԋu
#define MAINDLG_TIMER_ID_C	301			//�^�C�}�[ID (Ctrl���o)
#define MAINDLG_TIMER_TIME_C 100		//�^�C�}�[�Ԋu(Ctrl���o)

#define SIZE_WIDTH_WAKU		195			//���C���_�C�A���O�g��
#define SIZE_HEIGHT_SMALL	70			//���C���_�C�A���O�g���� �i�k�����j
#define SIZE_HEIGHT_FULL	150			//���C���_�C�A���O�g���� �i�g�厞�j

#define MAIN_DLG_WIDTH		312			//���C���_�C�A���O��

#define MAIN_DLG_HEIGHT_SMALL	105		//���C���_�C�A���O�����i�k�����j
#define MAIN_DLG_HEIGHT_FULL	185		//���C���_�C�A���O�����i�g�厞�j

#define MAIN_DLG_KEEP1ST_SH		21		//�P���߂͕��Ȃ� �E�C���h�E�̉�����̑��Έʒu
#define MAIN_DLG_KEEP1ST_LEFT	12		//�P���߂͕��Ȃ� ������̈ʒu

#define MAIN_DLG_PULL_LEFT		288		//�ڍו\�������{�b�N�X������̈ʒu

#define DEFAULT_SLEEP_TIME		"50"	//�P�������Ƃ̑ҋ@���ԕ�����i�~���b�j
#define MAX_SLEEP_TIME			200		//1�������Ƃ̑҂����Ԑݒ�ł���ő�l

#define MAX_APPEND_ITEMS	5			//�ǉ��A�C�e������5��

#define VERSION_CLEXP_MJ	2			//�o�[�W��������
#define VERSION_CLEXP_MM	7
#define VERSION_CLEXP_CH	""			//�o�[�W�����t��������

#define MODE_HOTKEY_NORMAL	0			//�z�b�g�L�[���[�h
#define MODE_HOTKEY_NOWIN	1			//

#define STR_CLEXP_MUTEX		"CLOSE_EXP"		//MUTEX
#define STR_CLEXP_CLASS		"CloseExpDummy"	//CLass
#define STR_CLEXP_WINTEXT	"CLSEXPDMMY"	//WindowText

#define STR_DATE			"2022/12/30"//�o�[�W����������
#define STR_VERSION			"CloseExp  Version %d.%d%s   (c)2001-2022 R.HOTTA   %s"

//#define STR_CLOSING			"���s��..."

#define STR_INIFILE			"clexp.ini"	//�ݒ�ۑ��t�@�C��
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

//�g���^�[�Q�b�g
typedef struct{
	char szName[128];	//�ݒ薼
	char szSpec[32];	//����ȏ���
	char szClass[128];	//�N���X��
	char szTitle[256];	//�E�C���h�E�^�C�g��
	BOOL fTarget;		//����Ώۂ��i��������Ă��邩�j
}CLEXP_APPEND, *pCLEXP_APPEND;

//�^�[�Q�b�g
typedef struct{
	BOOL Explorer;
	BOOL EdgeChrome;
	BOOL Firefox;
	CLEXP_APPEND APEND[MAX_APPEND_ITEMS];
}CLOSE_TARGET_WINDOW,*pCLOSE_TARGET_WINDOW;

//�ۑ�����ݒ�
typedef struct {
	BOOL fTopmost;

	CLOSE_TARGET_WINDOW tagTarget;
	BOOL fKepp1St;

	BOOL fFullSize;

	int iSleepTime;//�E�C���h�E�P�̑҂�����

	int iWinLeft;//���W�A���̒l�̂Ƃ��͎g�p����Ȃ�
	int iWinTop;

	int iHotKeyMode;

	BOOL fNotBackground;	//��풓���[�h
}CLEXP_SETTING, *pCLEXP_SETTING;



/*######################################################################
//  class C_MainDialog
// ���C���_�C�A���O
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
// �_�~�[�E�C���h�E
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
// �A�o�E�g�\���_�C�A���O
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
// �ݒ�_�C�A���O
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
// ���C���N���X
######################################################################*/
//�����o�֐��̏ڍׂ�cpp

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

	BOOL LoadSettings();//�ݒ�ǂݍ���
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

	BOOL m_MainDlgCreated;	//���łɕ\�����Ă���
	BOOL m_fRegistHotkey;	//�z�b�g�L�[��ݒ�ς�
//	int m_iHotKeyMode;

	BOOL m_bKeep1st;		//�ŏ��̃E�C���h�E�͖���(���s�����FALSE��)
	BOOL m_bCountOnly;		//���Ă݂̂���ꍇ
	BOOL m_bMinisize;		//�ŏ�������ꍇ

	CLOSE_TARGET_WINDOW m_Target;	//�^�[�Q�b�g

	int m_iNumOfEX;			//���ꂼ��̐�
	int m_iNumOfIE;
	int m_iNumOfNN;
	int m_aiNumOfAPPEND[MAX_APPEND_ITEMS];

	BOOL m_fFullSizeTemp=FALSE;	//�\���T�C�Y
							//�i�ꎞ�I�A�E�C���h�E������Ԃ����j�����݂̏��

	C_MainDialog	mc_maindlg;		//���C��DIALOG
	C_DammyWindow	mc_dammywindow;	//�E�C���h�E

	/*�ȉ��A�ۑ��K�v�Ȑݒ�*/
	CLEXP_SETTING m_tagSetting;

	C_CTRL_HYPERLINK cCLink;

};




