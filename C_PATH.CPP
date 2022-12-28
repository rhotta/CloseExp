////////////////////////////////////////////////////////////////////////
//
// FileName: c_path.cpp
//
// �t�@�C���A�f�B���N�g���p�X����p
//
//
//����� 01/02/01
//�C���� 06/07/11
//
// 2001 RyusukeHotta
#define _CRT_SECURE_NO_DEPRECATE 
#include <windows.h>
#include <shlobj.h>
#include <mbstring.h>
#include <tchar.h> 
#include "c_path.h"



/*##############################################################################
  CLASS CPath
##############################################################################*/
/*********************************************************
*	CPath()
*	�R���g���N�^
*********************************************************/
CPath::CPath(){
	;
}
/*********************************************************
*	GetExecutePath()
*	���s�f�B���N�g���𓾂�
*********************************************************/
TCHAR* CPath::GetExecutePath(LPTSTR lpPathname,DWORD nSize){
	GetModuleFileName(NULL,lpPathname,nSize);
	this->CutFileName(lpPathname);
	return lpPathname;
}

/*********************************************************
*	GetFullPath()
*	���s�f�B���N�g���ɔC�Ӄt�@�C����t���������p�X�𓾂�
*********************************************************/
TCHAR* CPath::GetFullPath(LPTSTR lpPath,DWORD nSize,const LPTSTR lpFilename){

	this->GetExecutePath(lpPath,nSize - _tcslen (lpFilename) -10);
	if(lpPath[_tcslen (lpPath)-1] != _T('\\')) _tcscat(lpPath,_T("\\"));
	_tcscat(lpPath,lpFilename);
	return lpPath;
}

/*********************************************************
*	GetSpecialFolderFullPath()
*	�w�����t�H���_�Ƀt�@�C�����𑫂������̂����߂�
*	MAX_PATH+�t�@�C�����ȏ���w�肷�邱��
*********************************************************/
TCHAR* CPath::GetSpecialFolderFullPath(LPTSTR lpPath,DWORD nSize,
		int iFolder,
		const LPTSTR lpFilename)
{
	SHGetSpecialFolderPath(NULL,lpPath,iFolder,TRUE);
	if(lpPath[_tcslen (lpPath)-1] != _T('\\')) _tcscat(lpPath,_T("\\"));
	_tcscat(lpPath,lpFilename);
	return lpPath;
}
/*********************************************************
*	CutFileName()
*	�p�X����t�@�C��������菜��
*   cPathChar = �f�B���N�g��������\
*********************************************************/
void CPath::CutFileName(TCHAR *lpPath,const TCHAR cPathChar){
    TCHAR *lpEnd=lpPath;
    
    while(lpPath[0]){
#ifndef _UNICODE
        //�Q�o�C�g�����̐擪�̓X�L�b�v
        if(IsDBCSLeadByte(*lpPath) == 0){
#endif
            if((*lpPath == cPathChar)){
                lpEnd=lpPath+1;
			}
#ifndef _UNICODE
        }
#endif
        lpPath=CharNext(lpPath);
    }
    //�p�X���Ƀt�@�C�����܂܂Ȃ������ꍇ�������Ȃ�
    if(lpEnd == lpPath) return;

    *lpEnd= _T('\0');
    return;
}

/*********************************************************
*	GetFilename()
*	�p�X����t�@�C�����̈ʒu�̃|�C���^���擾����
*   cPathChar = �f�B���N�g��������\
*********************************************************/
TCHAR* CPath::GetFilename(TCHAR *lpPath,const TCHAR cPathChar){
    TCHAR *lpEnd=lpPath;
    
    while(lpPath[0]){
#ifndef _UNICODE
        //�Q�o�C�g�����̐擪�̓X�L�b�v
        if(IsDBCSLeadByte(*lpPath) == 0){
#endif
            if((*lpPath == cPathChar)){
                lpEnd=lpPath+1;
            }
#ifndef _UNICODE
        }
#endif
        lpPath=CharNext(lpPath);
    }
    //�p�X���Ƀt�@�C�����܂܂Ȃ������ꍇ�������Ȃ�
    if(lpEnd == lpPath) return NULL;

    return lpEnd;
}
/*********************************************************
*	CutExt()
*	�p�X�܂��̓t�@�C��������g���q����菜��
*********************************************************/
TCHAR* CPath::CutExt(TCHAR* lpszPath){

	TCHAR *pLpoint = lpszPath;

	pLpoint +=_tcslen (lpszPath);
	pLpoint--;
	while(pLpoint[0] && (pLpoint >= lpszPath) ){
		pLpoint--;
		if(pLpoint[0] == _T('.')){
			pLpoint[0] = _T('\0');
			return pLpoint+1;
			break;
		}
	}

	return NULL;
}
/*********************************************************
*	CutExt()
*	�p�X�܂��̓t�@�C��������g���q����菜��
*********************************************************/
TCHAR* CPath::GetExt(TCHAR* lpszPath){

	TCHAR *pLpoint = lpszPath;

	pLpoint +=_tcslen (lpszPath);
	pLpoint--;
	while(pLpoint[0]){
		pLpoint--;
		if(pLpoint[0] == _T('.')){
			return pLpoint+1;
			break;
		}
	}

	return NULL;
}




BOOL IsExistFile(LPCTSTR lpszFileName, BOOL fDirectory)
{
    WIN32_FIND_DATA ffd;
    HANDLE  hFindFile = FindFirstFile(lpszFileName, &ffd);
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        // �f�B���N�g��
        if ( fDirectory &&
            (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            return TRUE;
        // �t�@�C��
        if ( !fDirectory &&
            !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            return TRUE;
    }
    return FALSE;
}

