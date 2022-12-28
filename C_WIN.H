/*
Win32 WindowManage Functions
By Ryusuke Hotta, 2001 Licensed under the MIT license
*/

/////////////////
//filename: c_win.h
//
//
//����� 01/02/01
//�C���� 01/10/11
//
//
/*

+-Ceditwindow
+---Cwindow		�ʏ�E�C���h�E
+---CMDialog	���[�_���_�C�A���O
+---CMLDialog	���[�_���X�_�C�A���O

+-CMDIwindow	MDI�E�C���h�E�p

*/
#define ERR_REGCLASS	-1



///////////////////////////////
//�����E�C���h�E����p
class Ceditwindow{
public:
	Ceditwindow();
	Ceditwindow(HWND hwnd);

	void SetTarget(HWND hwnd);//�^�[�Q�b�g�̐ݒ�

	//�E�C���h�E��\������
	//nWinMode		�\�����[�h
	void Show(int nWinMode=SW_SHOW);

	//�E�C���h�E�n���h���𓾂�
	HWND GetHandle(void);
#ifndef _WIN32_WCE
	HINSTANCE GetInst(void);
#endif
///////////////////////////////////
//�E�C���h�E�̏�񑀍�n

	//�E�C���h�E�̃T�C�Y��ύX����
	void SizeWindow(int w,int h);
#ifndef _WIN32_WCE
	//�E�C���h�E�T�C�Y���N���C�A���g�T�C�Y�w��ŕύX����
	void SizeWindowByClient(int w,int h);
#endif
	//�E�C���h�E�̈ړ��i��΍��W�w��j
	void MoveWindow(int x,int y);
	//�E�C���h�E�̈ړ��i���Ύw��j
	void MoveWindow2(int dx,int dy);

	//�����Ɉړ�
	void moveToCenter();

	//�������擾
	int GetClientWidth();
	int GetClientHeight();

	//�E�C���h�E�e�L�X�g�̐ݒ�
	void SetWindowText(TCHAR* title);

	//���[�W�����̐ݒ�
	void SetWindowRgn(HRGN hRgn,BOOL bRedraw);//SetWindowRgn

	//�őO�ʐݒ�
	void SetTopMost(BOOL topmost);


	//���b�Z�[�W���M
	BOOL PostMessage(UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT SendMessage(UINT Msg,WPARAM wParam,LPARAM lParam);

	//�^�C�}�[�֘A
	UINT SetTimer(UINT nIDEvent,UINT uElapse,TIMERPROC lpTimerFunc=NULL);
	BOOL KillTimer(UINT uIDEvent);
	//
///////////////////////////////////
//�_�C�A���O�֘A
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
	int	DeleteLine(int nIDDlgItem,int Index);//EditBox�̎w��s���폜

	LPVOID	GetParam(){return m_pParam;}
protected:
	HWND mHwnd;				//�E�C���h�E�n���h��

	LPVOID	m_pParam;		//�n�����߂̔C�ӂ̒l
};

///////////////////////////////
//�ʏ�E�C���h�E�p�N���X
class Cwindow:public Ceditwindow
{
public:
	Cwindow();
	~Cwindow();

	//�E�C���h�E�N���X�̓o�^�A
	//hThisInst		�C���X�^���X�n���h��
	//ClassName		�N���X���ipwcl���ȗ������ꍇ�j
	//lpszMenuName	���j���[�̎g�p�A�ȗ��� MAKEINTRESOURCE();
	//pwcl			�o�^����E�C���h�E���炷�̃|�C���^�B�ȗ���
#ifndef _UNICODE
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName
		,LPCTSTR lpszMenuName=NULL,WNDCLASSEX* pwcl=NULL,UINT style=0);
#else
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName
		,LPCTSTR lpszMenuName=NULL,WNDCLASS* pwcl=NULL,UINT style=0);
#endif

	//�E�C���h�E�̐���
	//hThisInst		����
	//hwnd			�e�E�C���h�E�i�f�X�N�g�b�v�� HWND_DESKTOP �j
	//dwStyle		�E�C���h�E�X�^�C���A�ȗ���
#ifndef _UNICODE
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,const char* title,
		DWORD dwStyle=WS_OVERLAPPEDWINDOW,DWORD dwExStyle = NULL);
#else
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,const TCHAR* title,
		DWORD dwStyle=WS_OVERLAPPEDWINDOW,DWORD dwExStyle = NULL);
#endif
	//�E�C���h�E��j������
	//�������f�o�C�X������Ύ����I�ɊJ������
	void DestroyWindow();

	//�f�o�C�X�R���e�L�X�g�n���h���𓾂�BReleaseDC�i���j�ŊJ�����邱��
	HDC GetDC();
	//����Ă񂾂��ƂɎg��
	void ReleaseDC(HDC hdc);

///////////////////////////////////
//�������f�o�C�X�n

	//�������f�o�C�X�R���e�L�X�g�𐻍삷��
	//������Paint()���g�p�ł���
	int CreateMemDC(int W,int H);
	int CreateMemDC();//<<���݂̃E�C���h�E�T�C�Y���g�p

	//��Ŋm�ۂ����f�o�C�X�R���e�L�X�g���폜����i�f�X�g���N�^�ł��Ă΂��j
	void DeleteMemDC(void);

	//�������f�o�C�X�R���e�L�X�g�𓾂�i�������Ă����ꍇ�j
	HDC GetMemDC();

	//WM_PAINT����������i���O��CreateMemDC�����s�j
	void Paint(void);

	//�������f�o�C�X����N���C�A���g�̈�փr�b�g�]��
	void ReDraw(void);

///////////////////////////////////
	//���z�E�C���h�E�v���[�W��
	virtual	LRESULT	WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );

	//���z���b�Z�[�W���[�v�֐�
	virtual int MessageLoop();

private:
protected:
	//�R�[���o�b�N�w��p�_�~�[�v���[�W��
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


	
	TCHAR mClassName[256];	//�E�C���h�E�N���X��

	//�������f�o�C�X�R���e�L�X�g�p
	HDC mMemDC;				//�f�o�C�X�R���e�L�X�g�n���h��
	HBITMAP  mHBit;			//�r�b�g�}�b�v�n���h��
	int mW;					//��
	int mH;					//����


};




///////////////////////////////
//���[�_���_�C�A���O�p�N���X
class CMDialog:public Ceditwindow
{
public:
	CMDialog(void);


	//���[�_���_�C�A���O���J��
	int Open(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent);
	//��Ɠ��������_�C�A���OID�𒼐ڎw��@�iMAKEINTRESOURCE���ȗ��j
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
//���[�_���X�_�C�A���O�p�N���X
class CMLDialog:public Ceditwindow
{
public:
	CMLDialog(void);


	//���[�_���X�_�C�A���O���J��
	BOOL Create(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent);
	//��Ɠ��������_�C�A���OID�𒼐ڎw��@�iMAKEINTRESOURCE���ȗ��j
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
//MDI�E�C���h�E�p
#define IDM_FIRSTCHILD 100
class CMDIwindow{


public:
	CMDIwindow(){;}
	~CMDIwindow(){DestroyMenu(m_hmenuDoc);}

	//�E�C���h�E�N���X�̓o�^�A
	//hThisInst		�C���X�^���X�n���h��
	//ClassName		�N���X��
	virtual int RegisterClass(HINSTANCE hThisInst,LPCTSTR ClassName,
		LPCTSTR DocWinClassName,UINT style=0);


	//�E�C���h�E�v���[�W��
	virtual	LRESULT	WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );

	//�h�L�������g�E�C���h�E�v���[�W��
	virtual	LRESULT	DocWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

	//�t���[���E�C���h�E�ƃN���C�A���g�E�C���h�E�����
	virtual int Create(HINSTANCE hThisInst,HWND hWndParent,TCHAR* title,
		LPTSTR lpszMenuName,LPTSTR lpszDocMenuName,
		int wmenu_1=0,int wmenu_2=0);

	//�h�L�������g�E�C���h�E�����
	virtual HWND CreateDocWin(LPTSTR title);

	//�A�N�e�B�u�ȃh�L�������g�E�C���h�E�Ƀ��b�Z�[�W�𑗂�
	//�t���[���E�C���h�E��WM_COMMAND�̃f�t�H���g�ȂǂŎg��
	BOOL SendMessageToActiveDocWin(UINT iMessage, WPARAM wParam, LPARAM lParam);

	//�h�L�������g�E�C���h�E��S�ĕ���
	void CloseAllDocWindow();

	//���ׂĕ\��
	void DocWnd_TILE(){SendMessage(m_hClient, WM_MDITILE, 0, 0);};
	//�d�˂ĕ\��
	void DocWnd_CASCADE(){SendMessage(m_hClient, WM_MDICASCADE, 0, 0);};
	//�A�C�R���̐���
	void DocWnd_ARRANGE(){SendMessage(m_hClient, WM_MDIICONARRANGE, 0, 0);};

	HINSTANCE GetInst(){return m_hThisInst;}


	//�N���C�A���g�����B�ĂԕK�v�Ȃ�
	HWND CreateClient(HWND hwnd);

	//���b�Z�[�W���[�v
	virtual int MessageLoop();

	HMENU GetMenu();
	HWND GetFrameHandle(){return m_hFrame;}
	HWND GetClientHandle(){return m_hClient;}
	int EVID_of_LastCreateListWin;	//�ꎞ�ۑ��p�@���X�g�������Ƀv���[�W���֓n��
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



	TCHAR mClassName[256];	//�E�C���h�E�N���X��
	TCHAR mDocWinClassName[256];	//�E�C���h�E�N���X��
	HINSTANCE m_hThisInst;
	HWND m_hClient;
	HWND m_hFrame;
};
#endif //end #ifndef _WIN32_WCE












