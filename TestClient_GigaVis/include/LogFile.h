#pragma once

#define LOG_FILE_PATH_LEN		512
#define LOG_FILE_NAME_LEN		100
#define LOG_FILE_FULL_PATH_LEN	512 + LOG_FILE_NAME_LEN
#define LOG_LEN					512
#define LOG_FILE_BACKUP_SIZE	10.0

class CLogFile
{
public:
	CLogFile();
	CLogFile(LPCTSTR strPath, LPCTSTR strFileName, CListCtrl *ctrList = NULL, LPCTSTR strCol = _T("Log"), int nListSize = 1000, int nMaxList = 50, BOOL isListTimeView = FALSE);
	~CLogFile();

private:
	void MakeDir(LPCTSTR path);
	

	TCHAR m_strFilePath[LOG_FILE_PATH_LEN];//파일 경로
	TCHAR m_strFileName[LOG_FILE_NAME_LEN];//파일 이름
	BOOL m_bLogCreate;
	BOOL m_bListCreate;
	CListCtrl *m_ctrListLog;
	int m_nMaxList;
	BOOL m_bListTimeView;

	CCriticalSection m_Critical;
	
public:
	BOOL Create(LPCTSTR strPath, LPCTSTR strFileName);//Log 생성
	BOOL WriteText(LPCTSTR strLog, BOOL isList = FALSE, long limit_file = 1000000);//Log 작성
	BOOL LogListInit(CListCtrl *ctrList, LPCTSTR strCol = _T("Log"), int nListSize = 1000, int nMaxList = 50, BOOL isListTimeView = FALSE);//List 초기화
	BOOL ListAllDelete();//List 내용 전체 삭제
	BOOL isLogCreate(){return m_bLogCreate;};
	BOOL isListCreate(){return m_bListCreate;};
};
