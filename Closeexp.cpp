////////////////////////////////////////////////////////////////////////
//
//FileName: closeexp.cpp
//�A�v���P�[�V����: CloseEXP
//
//����� 01/--/--
//�C���� 01/10/11
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


//������當������r
int strcmp_behind(const char* shorterstring, const char* longerstring)
{
	const char* p = longerstring + (strlen(longerstring) - strlen(shorterstring));
	return strcmp(shorterstring, p);
}


/*##############################################################################
  �O���[�o��
##############################################################################*/

/*//////////////////////////////////////////////////////////////////////
// EnumWindowsProcDammy()
// �E�C���h�E�񋓗p�A�R�[���o�b�N�֐��̃_�~�[
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
// C_CloseExp�R���g���N�^
/*//////////////////////////////////////////////////////////////////////
C_CloseExp::C_CloseExp()
{
	m_MainDlgCreated = FALSE;

	/*�ݒ�A�����l���*/
	m_tagSetting.fTopmost =  TRUE; //��Ɏ�O�ɕ\��
	m_tagSetting.fFullSize = FALSE;
	m_tagSetting.tagTarget.Explorer = TRUE;
	m_tagSetting.tagTarget.EdgeChrome =FALSE;
	m_tagSetting.tagTarget.Firefox = FALSE;
	for(int i=0;i<MAX_APPEND_ITEMS;i++){
		m_tagSetting.tagTarget.APEND[i].fTarget = FALSE;
	}
	m_tagSetting.fKepp1St = FALSE;

	m_tagSetting.iSleepTime = 50; //���邲�Ƃɂ��̒l�����҂�

	m_tagSetting.iWinLeft = -1;
	m_tagSetting.iWinTop = -1;

	m_tagSetting.iHotKeyMode = MODE_HOTKEY_NORMAL;
	m_tagSetting.fNotBackground = FALSE;

	m_fRegistHotkey = FALSE;
}
/*//////////////////////////////////////////////////////////////////////
// StartUp()
// �G���g���߲�āiWinMain�ŌĂяo���j1�x�̂݌Ăяo�����
/*//////////////////////////////////////////////////////////////////////
int
C_CloseExp::StartUp(HINSTANCE hThisInst,LPSTR lpszArgs)
{
	BOOL fFirstRun;
	m_hHinst =hThisInst;
	mc_maindlg.SetCloseExpClassAddress((void*)this);

	/*�J�X�^���R���g���[���p*/
	cCLink.RegisterClass(hThisInst,"KNOHLinkClass");


	/*�ݒ�̓ǂݍ���*/
	fFirstRun = this->LoadSettings();
		
	
	if(m_tagSetting.fNotBackground){	/*��풓���[�h�̏ꍇ*/
		/*�_�C�A���O�\��*/
		ShowMainDialog();

		/*�ݒ�̕ۑ�*/
		SaveSettings();

		return 0;
	}else{								/*�ʏ�i�풓�j���[�h�̏ꍇ*/

		/*MUTEX�̓o�^*/
		HANDLE hMutex=::CreateMutex(NULL,TRUE,STR_CLEXP_MUTEX);
		if(!hMutex) return 0;
		if(::GetLastError() == ERROR_ALREADY_EXISTS){
			HWND hPrev  = ::FindWindow(STR_CLEXP_CLASS,STR_CLEXP_WINTEXT);
			if(hPrev){
				::PostMessage(hPrev,WM_CLEXP_SHOWDLG,0,0);
				return 0; /*2�d�N���͂��Ȃ�*/
			}
		}

		/*�_�~�[�E�C���h�E�̐���*/
		mc_dammywindow.SetCloseExpClassAddress((void*)this);
		mc_dammywindow.RegisterClass(
			hThisInst,STR_CLEXP_CLASS
			);
		mc_dammywindow.Create(hThisInst,NULL,STR_CLEXP_WINTEXT);


		/*�z�b�g�L�[�̐ݒ�*/
		this->RegisterHotkeys();

		/*�^�X�N�g���C�A�C�R���o�^*/
		this->TasktrayAdd();

		/*�ŏ��̋N���Ȃ�΃_�C�A���O�\��*/
		if(fFirstRun)//this->ShowMainDialog();
			OnAboutCloseExp(HWND_DESKTOP);
	}

	/*���b�Z�[�W���[�v*/
	return mc_dammywindow.MessageLoop();


}
/*//////////////////////////////////////////////////////////////////////
// CloseTargetWindows()
// �E�C���h�E�����
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
// �_�C�A���O�A�ȈՕ\���A�t���\������ς�
/*//////////////////////////////////////////////////////////////////////
void
C_CloseExp::AdjustDialogSize(HWND hwnd,BOOL fFullSize){
	RECT rt;
	POINT pt;
	
	/*���݂̏�Ԃ�ێ�*/
	m_fFullSizeTemp=fFullSize;

	/*�X�N���[�����W�ϊ�*/
	::GetWindowRect(::GetDlgItem(hwnd,IDC_STATIC_MW),&rt);
	pt.x = rt.left;pt.y=rt.top;ScreenToClient(hwnd,&pt);
	rt.left = pt.x;rt.top =pt.y;

	if(fFullSize){	//�傫���T�C�Y

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


	}else{	//�������T�C�Y
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
// ���C���_�C�A���O�������ɌĂяo�����
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

	//�o�[�W�����\��
//	::wsprintf(szBuf,"v%d.%d%s",VERSION_CLEXP_MJ,VERSION_CLEXP_MM,VERSION_CLEXP_CH);
	::SetDlgItemText(hwnd,IDC_STATIC_VER,"");

	HFONT hFont=(HFONT)::SendMessage(::GetDlgItem(hwnd,IDC_STATIC_VER),WM_GETFONT,0,0);
	cCLink.SetFont(hFont);
	::SendDlgItemMessage(hwnd,IDC_CUSTOM_VERSION,WM_HLINKALING_RIGHT,0,0);
	::SendDlgItemMessage(hwnd,IDC_CUSTOM_VERSION,WM_HLINKALING_TARGET,(WPARAM)hwnd,IDC_CUSTOM_VERSION);

	//�_�C�A���O�ʒu����
	::GetWindowRect(::GetDesktopWindow(),&rt);
	if(m_tagSetting.iWinLeft >= 0 && m_tagSetting.iWinTop >=0
		&&m_tagSetting.iWinLeft < rt.right && m_tagSetting.iWinTop < rt.bottom){
		::MoveWindow(hwnd,m_tagSetting.iWinLeft,m_tagSetting.iWinTop,10,10,FALSE);
	}

	//�_�C�A���O�T�C�Y����
	AdjustDialogSize(hwnd,m_tagSetting.fFullSize);

	//������Ԃ̕���
	if(m_tagSetting.tagTarget.Explorer) mc_maindlg.SetCheckItem(IDC_CHECK1,TRUE);
	if(m_tagSetting.tagTarget.EdgeChrome) mc_maindlg.SetCheckItem(IDC_CHECK2,TRUE);
	if(m_tagSetting.tagTarget.Firefox) mc_maindlg.SetCheckItem(IDC_CHECK3,TRUE);
	int i;
	for(i=0;i<MAX_APPEND_ITEMS;i++){
			if(m_tagSetting.tagTarget.APEND[i].fTarget)
				mc_maindlg.SetCheckItem(k_DlgItemCheck[i],TRUE);
	}
	if(m_tagSetting.fKepp1St) mc_maindlg.SetCheckItem(IDC_CHECK4,TRUE);

	//�ǉ����ڂ̖��O������ݒ�
	LoadAllTarget();
	for(i=0;i<MAX_APPEND_ITEMS;i++){
		::SetDlgItemText(hwnd,k_DlgItemCheck[i],m_Target.APEND[i].szName);
	}

	//TOPMOST����
	if(m_tagSetting.fTopmost) {
		mc_maindlg.SetCheckItem(IDC_CHECK5,TRUE);
	}
	OnSetTopmost(hwnd);


	/*�^�C�}�[���Z�b�g����*/
	::SetTimer(hwnd,MAINDLG_TIMER_ID,MAINDLG_TIMER_TIME,NULL);
	::SetTimer(hwnd,MAINDLG_TIMER_ID_C,MAINDLG_TIMER_TIME_C,NULL);

	/*����1��ڂ����s*/
	CountWIndowNum(hwnd);

	/*��O�ɏo��*/
	::SetWindowPos(hwnd, HWND_TOP ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	::SetForegroundWindow(hwnd);

	return FALSE;
}


/*//////////////////////////////////////////////////////////////////////
// OnHotkey()
// �z�b�g�L�[�������ꂽ�Ƃ�
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
// ���C���_�C�A���O�I�����ɌĂяo�����
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::OnMainDlgEnd(HWND hwnd,BOOL fRun)
{
	char szBuf[200];

	/*�^�C�}�[���폜����*/
	::KillTimer(hwnd,MAINDLG_TIMER_ID);
	::KillTimer(hwnd,MAINDLG_TIMER_ID_C);

	/*�L�[��Ԏ擾*/
	BOOL fContrlOn = (GetAsyncKeyState(VK_CONTROL) &0x8000) ? TRUE : FALSE;

	

	if(fRun || fContrlOn){

		/*�`�F�b�N�{�b�N�X��Ԃ𓾂�*/
		m_tagSetting.tagTarget.Explorer = mc_maindlg.IsCheckedItem(IDC_CHECK1);
		m_tagSetting.tagTarget.EdgeChrome = mc_maindlg.IsCheckedItem(IDC_CHECK2);
		m_tagSetting.tagTarget.Firefox = mc_maindlg.IsCheckedItem(IDC_CHECK3);
	//	if(m_fFullSizeTemp){
	//		/*�g���\���ɂȂ��Ă���Ƃ��̂�*/
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
		/*�E�C���h�E��Ԃ�ۑ�*/
		m_tagSetting.fFullSize = m_fFullSizeTemp;
	}
	/*�E�C���h�E�ʒu�ۑ�*/
	RECT rt;
	::GetWindowRect(hwnd,&rt);
	m_tagSetting.iWinLeft = rt.left;
	m_tagSetting.iWinTop = rt.top;

	/*�ݒ�̕ۑ�*/
	SaveSettings();

	return FALSE;
}
/*//////////////////////////////////////////////////////////////////////
// OnSetTopmost()
// �g�b�v���X�g �ݒ�A�����̓K�p
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
// �E�C���h�E�̌��𐔂��ă_�C�A���O���̒l���X�V
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
// �A�o�E�g
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnAboutCloseExp(HWND hwnd)
{
	C_ABOUTDLG c_about;
	c_about.SetCloseExpClassAddress((void*)this);
	if(c_about.OpenRes(m_hHinst,IDD_DIALOG2,hwnd))
		return TRUE; //�X�^�[�g�A�b�v�ɓo�^�{�^�����������̂Ń��C���_�C�A���O����
	else  return FALSE;

}

/*//////////////////////////////////////////////////////////////////////
// OnSetting()
// �ݒ�J��
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnSetting(HWND hwnd)
{
	C_SETTINGDLG c_setting;
	c_setting.SetCloseExpClassAddress((void*)this);
	c_setting.OpenRes(m_hHinst,IDD_DIALOGSETTING,hwnd);
	/*�ݒ�̕ۑ�*/
	SaveSettings();
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// TasktrayAdd()
// �^�X�N�g���C�o�^
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
// �^�X�N�g���C����폜
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
// �E�C���h�E�񋓁A�R�[���o�b�N�֐�����̌Ăяo��
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::EnumWindowsProc(HWND hwnd)
{
	char szClassName[256];
	char szWindowName[256];
	::GetClassName(hwnd,szClassName,sizeof(szClassName));
	::GetWindowText(hwnd,szWindowName,sizeof(szWindowName));
	BOOL fClosed=FALSE;

	/*�G�N�X�v���[���E�C���h�E�`�F�b�N*/
	if(this->IsEX(hwnd,szClassName,szWindowName)){
		m_iNumOfEX++;
		if(m_Target.Explorer) fClosed = CloseTargetWindows(hwnd);
	}

	/*IE�`�F�b�N*/
	if(this->IsEdgeOrChrome(hwnd,szClassName,szWindowName)){
		m_iNumOfIE++;
		if(m_Target.EdgeChrome) fClosed = CloseTargetWindows(hwnd);
	}

	/*NN�`�F�b�N*/
	if(this->IsFirefox(hwnd,szClassName,szWindowName)){
		m_iNumOfNN++;
		if(m_Target.Firefox) fClosed = CloseTargetWindows(hwnd);
	}

	/*�g���^�[�Q�b�g�`�F�b�N*/
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
// �S�ẴE�C���h�E�����i�t���O�Ɋ�Â��j
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
// �G�N�X�v���[���ł��邩�ǂ���������
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
// IE�ł��邩�ǂ���������
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsFirefox(HWND hwnd,const LPSTR szClassName,const LPSTR szWindowName)
{

	/* ���o�[�W����
	if (szClassName[0] == 'A'
		&& szClassName[1] == 'f'
		&& szClassName[2] == 'x'
		&& szClassName[3] == ':') {
		//NN4.x��*Afx:�Ŏn�܂�
		if (::strstr(szWindowName, "Netscape")) {
			//"Netscape"���E�C���h�E�e�L�X�g�Ɋ܂�
			//�ȉ��ANetscape���b�Z���W���[�Ƃ̋��
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
		//6.0��"MozillaWindowClass"���N���X���Ɏ���
		if (::GetWindowLong(hwnd, GWL_STYLE) == 0x14CF0000) {
			//���j���[�Ȃǂ������N���X���ł���̂ŁA�X�^�C�����`�F�b�N
			return TRUE;//NN6�ł���
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
// NN�ł��邩�ǂ���������
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::IsEdgeOrChrome(HWND hwnd, const LPSTR szClassName, const LPSTR szWindowName)
{

	if (::strcoll("Chrome_WidgetWin_1", szClassName) == 0) {
		if (::strcoll("", szWindowName) == 0) { //Edge�̉B���E�C���h�E
			return FALSE;
		}
		if (::strcoll("Tray Assistant", szWindowName) == 0) { //GoogleChrome�̏풓�g���C
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
// �^�[�Q�b�g�ł��邩�ǂ���������
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
		//�S�ẴE�C���h�E�����
		if(::IsWindowVisible(hwnd)&&::IsWindowEnabled(hwnd)){
			DWORD dwStyle = ::GetWindowLong(hwnd,GWL_STYLE);
			RECT rt;
			/*�T�C�Y���O�̃E�C���h�E�͖�������*/
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
		//�O���s��v
		if(!strstr(szWindowName,&(m_Target.APEND[iTargetNo].szTitle[1]))){
			fTitleMatch = TRUE;
		}
	}else{
		//�O����v
		if(strstr(szWindowName,m_Target.APEND[iTargetNo].szTitle)){
			fTitleMatch = TRUE;
		}
	}
	fRet = (fClassMatch && fTitleMatch && fAllWindow);
	
	return fRet;
}
/*//////////////////////////////////////////////////////////////////////
// CloseWindow()  
// �����̃E�C���h�E�����
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::CloseWindow(HWND hwnd)
{
	::PostMessage(hwnd,WM_SYSCOMMAND,SC_CLOSE,0);
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////
// CloseWindow()  
// �����̃E�C���h�E���ŏ�������
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
// �z�b�g�L�[��o�^����
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
// �z�b�g�L�[����������
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
// �^�X�N�g���C�R�[���o�b�N�֐�
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
				//ID_MENU_KEEP1ST = ��ԏ�͕��Ȃ�
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
// �I������1�x�����Ăяo�����
/*//////////////////////////////////////////////////////////////////////
BOOL 
C_CloseExp::OnExitCloseExp()
{
	/*�^�X�N�g���C����*/
	this->TasktrayDel();

	/*�z�b�g�L�[����*/
	this->UnRegisterHotkeys();

	/*�ݒ�̕ۑ�*/
	this->SaveSettings();

	::PostQuitMessage(0);
//	mc_dammywindow.SendMessage(WM_DESTROY,0,0);
	return 0;
}



/*//////////////////////////////////////////////////////////////////////
// ShowMainDialog()
// ���C���_�C�A���O�̕\��
/*//////////////////////////////////////////////////////////////////////
BOOL
C_CloseExp::ShowMainDialog(void)
{
	if(m_MainDlgCreated) {//���ɕ\����
			/*��O�ɏo��*/
		::SetWindowPos(mc_maindlg.GetHandle(), HWND_TOP ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		::SetForegroundWindow(mc_maindlg.GetHandle());
		return FALSE;
	}
	m_MainDlgCreated = TRUE;
	mc_maindlg.OpenRes(m_hHinst,IDD_DIALOG1,NULL);
	m_MainDlgCreated = FALSE;

	/*�v���Z�X�T�C�Y������������*/
	::SetProcessWorkingSetSize(GetCurrentProcess(),0xffffffff, 0xffffffff );
	return TRUE;
}
/*//////////////////////////////////////////////////////////////////////
// LoadSettings()
// �Z�b�e�B���O�̓ǂݍ���
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
// �Z�b�e�B���O�̕ۑ�
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
// �^�[�Q�b�g�t�@�C����ǂݍ���
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
// �^�[�Q�b�g�t�@�C�����̑S�ēǂݍ���
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
// ���C���_�C�A���O�v���[�W���֐�
//---------------------------------------------------------------------
LRESULT CALLBACK 
C_MainDialog::DlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP){
	static C_CloseExp *c_closeexp = (C_CloseExp*)m_clsexp;
	static BOOL fFirst =TRUE;
	switch (uMsg) {
	case WM_INITDIALOG:

		/*�_�C�A���O������*/
		return c_closeexp->OnMainDlgInit(hwnd,uMsg,wP,lP);
	case WM_TIMER:
		switch((int)wP){
		case MAINDLG_TIMER_ID://���݂̐�����
			c_closeexp->CountWIndowNum(hwnd);break;
		case MAINDLG_TIMER_ID_C:
			if(fFirst){	//1�x�������s
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
		case IDC_CHECK5:	//�ŏ�ʌŒ�N���b�v
			c_closeexp->OnSetTopmost(hwnd);
			break;
		case IDC_STATIC_PULL:
			c_closeexp->AdjustDialogSize(hwnd,TRUE);
			break;
		case IDC_STATIC_PUSH:
			c_closeexp->AdjustDialogSize(hwnd,FALSE);
			break;
		case IDOK:

			/*�_�C�A���O�̏I��*/
			c_closeexp->OnMainDlgEnd(hwnd,TRUE);
			::EndDialog(hwnd,1);
			break;
		case IDCHHIDE:
		case IDCANCEL:
			/*�_�C�A���O�̏I��*/
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
// �R���g���[���L�[�̏�Ԃɍ��킹�ă{�^���̕�����ω�
//---------------------------------------------------------------------
void C_MainDialog::ChangeBtnString(HWND hwnd){

	char szBuf[32];
	static BOOL fPrevState = FALSE;//�P�O�̏�ԁ@FALSE=�ʏ�

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
// �_�~�[�E�C���h�E�A�v���[�W��
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
	case WM_CLEXP_SHOWDLG://���C���_�C�A���O�\��
		c_closeexp->ShowMainDialog();
		break;
	case WM_HOTKEY://�z�b�g�L�[�������ꂽ�Ƃ�
		c_closeexp->OnHotkey((int)wP);
		break;
	case WM_COMMAND:

		switch (LOWORD(wP)){
		case WM_ENDSESSION :
		case ID_MENU_EXIT://�I��
			return c_closeexp->OnExitCloseExp();
		case ID_MENU_KEEP1ST://�ŏ��̃E�C���h�E�͕��Ȃ�
			c_closeexp->OnMenuKeep1st();
			break;
		case ID_MENU_CLOSEEX://�G�N�X�v���[������
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSEEX,0);
			break;
		case ID_MENU_CLOSEIE://IE����
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSEIE,0);
			break;
		case ID_MENUCLOSENN://NN����
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLOSENN,0);
			break;
		case ID_MENU_RUN_MIN://���݂̐ݒ�ł��ׂčŏ���
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_MIN_BYSET,0);
			break;
		case ID_MENU_RUN_CLS://���݂̐ݒ�ł��ׂĕ���B
			this->PostMessage(WM_HOTKEY,HOTKEY_ID_CLS_BYSET,0);
			break;
		case ID_MEN_OPEN://���C���E�C���h�E���J��
			c_closeexp->ShowMainDialog();
			break;
		case ID_MENU_ABOUT://�A�o�E�g
			c_closeexp->OnAboutCloseExp(NULL);
			break;
		case ID_MENUITEMSETTING://�ݒ�
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
			//�o�[�W�����\��
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