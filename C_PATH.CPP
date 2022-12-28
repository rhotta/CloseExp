////////////////////////////////////////////////////////////////////////
//
// FileName: c_path.cpp
//
// ファイル、ディレクトリパス操作用
//
//
//制作日 01/02/01
//修正日 06/07/11
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
*	コントラクタ
*********************************************************/
CPath::CPath(){
	;
}
/*********************************************************
*	GetExecutePath()
*	実行ディレクトリを得る
*********************************************************/
TCHAR* CPath::GetExecutePath(LPTSTR lpPathname,DWORD nSize){
	GetModuleFileName(NULL,lpPathname,nSize);
	this->CutFileName(lpPathname);
	return lpPathname;
}

/*********************************************************
*	GetFullPath()
*	実行ディレクトリに任意ファイルを付け足したパスを得る
*********************************************************/
TCHAR* CPath::GetFullPath(LPTSTR lpPath,DWORD nSize,const LPTSTR lpFilename){

	this->GetExecutePath(lpPath,nSize - _tcslen (lpFilename) -10);
	if(lpPath[_tcslen (lpPath)-1] != _T('\\')) _tcscat(lpPath,_T("\\"));
	_tcscat(lpPath,lpFilename);
	return lpPath;
}

/*********************************************************
*	GetSpecialFolderFullPath()
*	指定特殊フォルダにファイル名を足したものを求める
*	MAX_PATH+ファイル名以上を指定すること
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
*	パスからファイル名を取り除く
*   cPathChar = ディレクトリ文字列＝\
*********************************************************/
void CPath::CutFileName(TCHAR *lpPath,const TCHAR cPathChar){
    TCHAR *lpEnd=lpPath;
    
    while(lpPath[0]){
#ifndef _UNICODE
        //２バイト文字の先頭はスキップ
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
    //パス名にファイルを含まなかった場合何もしない
    if(lpEnd == lpPath) return;

    *lpEnd= _T('\0');
    return;
}

/*********************************************************
*	GetFilename()
*	パスからファイル名の位置のポインタを取得する
*   cPathChar = ディレクトリ文字列＝\
*********************************************************/
TCHAR* CPath::GetFilename(TCHAR *lpPath,const TCHAR cPathChar){
    TCHAR *lpEnd=lpPath;
    
    while(lpPath[0]){
#ifndef _UNICODE
        //２バイト文字の先頭はスキップ
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
    //パス名にファイルを含まなかった場合何もしない
    if(lpEnd == lpPath) return NULL;

    return lpEnd;
}
/*********************************************************
*	CutExt()
*	パスまたはファイル名から拡張子を取り除く
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
*	パスまたはファイル名から拡張子を取り除く
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
        // ディレクトリ
        if ( fDirectory &&
            (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            return TRUE;
        // ファイル
        if ( !fDirectory &&
            !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            return TRUE;
    }
    return FALSE;
}


