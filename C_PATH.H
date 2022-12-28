////////////////////////////////////////////////////////////////////////
//
// FileName: c_path.h
//
// ファイル、ディレクトリパス操作用
//
//制作日 01/--/--
//修正日 01/10/11
//
// 2001 RyusukeHotta


class CPath{

public:
	CPath();

	//実行ディレクトリを得る
	TCHAR *GetExecutePath(LPTSTR lpPathname,DWORD nSize);

	//実行ディレクトリにファイル名を足したものを求める
	TCHAR *GetFullPath(LPTSTR lpPath,DWORD nSize,
		const LPTSTR lpFilename);

	//指定特殊フォルダにファイル名を足したものを求める
	//#include <shlobj.h>が必要
	//MAX_PATH+ファイル名以上を指定すること
	//CSIDL_APPDATA
	TCHAR *GetSpecialFolderFullPath(LPTSTR lpPath,DWORD nSize,
		int iFolder,
		const LPTSTR lpFilename);


	//パスからファイル名を取り除く  最後は\で終わる
	void CutFileName(TCHAR *lpszPath,const TCHAR cPathChar=TEXT('\\'));

	//パスからファイル名の位置のポインタを取得する
	TCHAR* GetFilename(TCHAR *lpszPath,const TCHAR cPathChar=TEXT('\\'));
	
	//ファイル名（またはパス）から拡張しを取り除く 戻り値=拡張子
	TCHAR* CutExt(TCHAR* lpszPath);		

	//拡張子の位置へのポインタを取得する
	TCHAR* GetExt(TCHAR* lpszPath);	


};



//ファイル存在確認 fDirectoryでディレクトリも含める
BOOL IsExistFile(LPCTSTR lpszFileName, BOOL fDirectory = FALSE);

