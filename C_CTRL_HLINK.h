

/*
����
 1.RegisterClass() �ŃN���X���o�^ "KNOHLinkClass"
 2.�t�H���g�𑼂̃A�C�e���Ƃ��킹��
	HFONT hFont=(HFONT)::SendMessage(GetDlgItem(hwnd,IDC_STATIC_Q),WM_GETFONT,0,0);
	cCLink.SetFont(hFont);
 3.�e�L�X�g�z�u�A���b�Z�[�W�󂯎��E�C���h�E�Ȃǂ�ݒ�@WM_INITDIALOG
  	SendDlgItemMessage(IDC_CUSTOM1,WM_HLINKALING_RIGHT,0,0);
	SendDlgItemMessage(IDC_CUSTOM1,WM_HLINKALING_TARGET,(WPARAM)hwnd,IDC_CUSTOM1);

*/

#define WM_HLINKALING_LEFT	WM_USER + 10
#define WM_HLINKALING_RIGHT	WM_USER + 11



//wp�ɃE�C���h�E�n���h���w��,lp��ID�w��œ�����
//�N���b�N���ꂽ�Ƃ��̂��̃n���h����WM_HLINKALING_TARGET��Ԃ� lp ��ID�������Ă���
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