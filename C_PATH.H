////////////////////////////////////////////////////////////////////////
//
// FileName: c_path.h
//
// �t�@�C���A�f�B���N�g���p�X����p
//
//����� 01/--/--
//�C���� 01/10/11
//
// 2001 RyusukeHotta


class CPath{

public:
	CPath();

	//���s�f�B���N�g���𓾂�
	TCHAR *GetExecutePath(LPTSTR lpPathname,DWORD nSize);

	//���s�f�B���N�g���Ƀt�@�C�����𑫂������̂����߂�
	TCHAR *GetFullPath(LPTSTR lpPath,DWORD nSize,
		const LPTSTR lpFilename);

	//�w�����t�H���_�Ƀt�@�C�����𑫂������̂����߂�
	//#include <shlobj.h>���K�v
	//MAX_PATH+�t�@�C�����ȏ���w�肷�邱��
	//CSIDL_APPDATA
	TCHAR *GetSpecialFolderFullPath(LPTSTR lpPath,DWORD nSize,
		int iFolder,
		const LPTSTR lpFilename);


	//�p�X����t�@�C��������菜��  �Ō��\�ŏI���
	void CutFileName(TCHAR *lpszPath,const TCHAR cPathChar=TEXT('\\'));

	//�p�X����t�@�C�����̈ʒu�̃|�C���^���擾����
	TCHAR* GetFilename(TCHAR *lpszPath,const TCHAR cPathChar=TEXT('\\'));
	
	//�t�@�C�����i�܂��̓p�X�j����g��������菜�� �߂�l=�g���q
	TCHAR* CutExt(TCHAR* lpszPath);		

	//�g���q�̈ʒu�ւ̃|�C���^���擾����
	TCHAR* GetExt(TCHAR* lpszPath);	


};



//�t�@�C�����݊m�F fDirectory�Ńf�B���N�g�����܂߂�
BOOL IsExistFile(LPCTSTR lpszFileName, BOOL fDirectory = FALSE);
