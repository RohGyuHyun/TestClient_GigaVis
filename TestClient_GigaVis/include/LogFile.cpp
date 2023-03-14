#include "pch.h"
#include "locale.h"
#include "LogFile.h"

#ifdef  _UNICODE
	#define	_tsprintf	wsprintf
	#define _tfopen		_wfopen
	#define _tfputs		fputws
#else
	#define	_tsprintf	sprintf
	#define _tfopen		fopen
	#define _tfputs		fputs
#endif

CLogFile::CLogFile()
{
	memset(m_strFilePath, NULL, sizeof(m_strFilePath));
	memset(m_strFileName, NULL, sizeof(m_strFileName));
	m_bLogCreate = FALSE;
	m_bListCreate = FALSE;
	m_ctrListLog = NULL;
	m_nMaxList = 0;
	m_bListTimeView = FALSE;
}

CLogFile::CLogFile(LPCTSTR strPath, LPCTSTR strFileName, CListCtrl *ctrList, LPCTSTR strCol, int nListSize, int nMaxList, BOOL isListTimeView)
{
#ifdef _UNICODE
	_wsetlocale(LC_ALL, L"korean");
#endif
	memset(m_strFilePath, NULL, sizeof(m_strFilePath));
	memset(m_strFileName, NULL, sizeof(m_strFileName));
	m_bLogCreate = FALSE;
	m_bListCreate = FALSE;
	m_ctrListLog = NULL;
	m_nMaxList = 0;
	m_bListTimeView = FALSE;
	Create(strPath, strFileName);
	if(ctrList != NULL)
	{
		LogListInit(ctrList, strCol, nListSize, nMaxList, isListTimeView);
	}
}

CLogFile::~CLogFile()
{

}

void CLogFile::MakeDir(LPCTSTR path)
{
	int nPathSize = (int)_tcslen(path);
	long index = 0;
	TCHAR tmp_path[LOG_FILE_PATH_LEN];
	TCHAR sub[LOG_FILE_PATH_LEN];
	memset(sub, NULL, sizeof(sub));
	_tsprintf(tmp_path, _T("%s"), path);

	while(TRUE)
	{
		if(tmp_path[index] == '\\')
		{
			CreateDirectory(sub, NULL);
		}

		sub[index] = tmp_path[index];

		if(tmp_path[index] == '\0')
		{
			CreateDirectory(sub, NULL);
			break;
		}

		index++;

		if( (index > LOG_FILE_PATH_LEN) )
		{
			AfxMessageBox(_T("Error : index overflow!"));
			break;
		}
	}
}

BOOL CLogFile::Create(LPCTSTR strPath, LPCTSTR strFileName)
{
	if(_tcslen(strPath) == 0)
	{
		AfxMessageBox(_T("FilePath String NULL !!"));
		return m_bLogCreate;
	}	

	if(_tcslen(strFileName) == 0)
	{
		AfxMessageBox(_T("FileName String NULL !!"));
		return m_bLogCreate;
	}

	m_bLogCreate = TRUE;

	_tsprintf(m_strFilePath, _T("%s"), strPath);
	_tsprintf(m_strFileName, _T("%s"), strFileName);

	MakeDir(m_strFilePath);

	return m_bLogCreate;
}


BOOL CLogFile::WriteText(LPCTSTR strLog, BOOL isList, long limit_file)
{
	if(!m_bLogCreate)
	{
		AfxMessageBox(_T("CLogFile Class Create Fail !!"));
		return FALSE;
	}
	m_Critical.Lock();
	FILE*	stream;
    long	result;
	TCHAR strFile[LOG_FILE_FULL_PATH_LEN];
	TCHAR strFullLog[LOG_LEN];
	SYSTEMTIME	lpSystemTime;

	GetLocalTime(&lpSystemTime);
	MakeDir(m_strFilePath);

	_tsprintf(strFile, _T("%s%04d%02d%02d_%s"), m_strFilePath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, m_strFileName); 

	if((stream = _tfopen( strFile, _T("a+") )) == NULL)
	{
#ifdef _UNICODE
		HANDLE fd = CreateFile( (LPCWSTR)strFile,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
#else
		HANDLE fd = CreateFile( (LPCSTR)strFile,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
#endif
		stream = _tfopen( strFile, _T("a+") );
		CloseHandle(fd);
	}

	result = fseek( stream, 0L, SEEK_END);

    if(result) TRUE;
	double nFileLength = (double)ftell(stream) / 1024. / 1024.;
	int nBackIndex = 0;

	if(nFileLength > LOG_FILE_BACKUP_SIZE)
	{
		fclose(stream);
		CString strFileBackupPath, strSrcFileNamePath, strTemp1, strTemp2, strFileName;
		strFileName.Format(_T("%04d%02d%02d_%s"), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, m_strFileName);
		strTemp1.Format(_T("%s"), strFile);

		
		while(TRUE)
		{
			strTemp2.Format(_T("%s_%d.log"), strFileName.Mid(0, strFileName.GetLength() - 4), nBackIndex); 

			if(!FileSearch(m_strFilePath, strTemp2))
			{
				strFileBackupPath.Format(_T("%s%s"), m_strFilePath, strTemp2);
				if(CopyFile(strTemp1, strFileBackupPath, FALSE))
				{
					DeleteFile(strTemp1);
				}
				break;
			}
			nBackIndex++;
		}

		if((stream = _tfopen( strFile, _T("a+") )) == NULL)
		{
#ifdef _UNICODE
			HANDLE fd = CreateFile( (LPCWSTR)strFile,
								GENERIC_READ|GENERIC_WRITE,
								0,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL
								);
#else
			HANDLE fd = CreateFile( (LPCSTR)strFile,
								GENERIC_READ|GENERIC_WRITE,
								0,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL
								);
#endif
			stream = _tfopen( strFile, _T("a+") );
			CloseHandle(fd);
		}

		result = fseek( stream, 0L, SEEK_END);

		if(result) TRUE;

	}

	GetLocalTime(&lpSystemTime);
	memset(strFullLog, NULL, sizeof(strFullLog));

	//_tsprintf(strFullLog, _T("%04d-%02d-%02d %02d:%02d:%02d - %s"), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, strLog);
	_tsprintf(strFullLog, _T("%s"), strLog);

    _tfputs(strFullLog, stream);
	_tfputs(_T("\n"), stream);

	fclose(stream);

	if(isList)
	{
		if(!m_bListCreate || (m_ctrListLog == NULL))
		{
			AfxMessageBox(_T("ListCtrl Create Fail !!"));
			return FALSE;
		}

		if(m_nMaxList <= m_ctrListLog->GetItemCount())
		{
			m_ctrListLog->DeleteItem(m_nMaxList - 1);
		}

		if(m_bListTimeView)
		{
			_tsprintf(strFullLog, _T("%02d:%02d:%02d - %s"), lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, strLog);
		}
		else
		{
			_tsprintf(strFullLog, _T("%s"), strLog);
		}
		m_ctrListLog->InsertItem(0, strFullLog);
	}
	m_Critical.Unlock();
	return TRUE;
}

BOOL CLogFile::LogListInit(CListCtrl *ctrList, LPCTSTR strCol, int nListSize, int nMaxList, BOOL isListTimeView)
{
	if(ctrList == NULL)
	{
		AfxMessageBox(_T("ListCtrl Create Fail !!"));
		return m_bListCreate;
	}

	TCHAR sCol[LOG_LEN];
	_tsprintf(sCol, _T("%s"), strCol);
	LV_COLUMN m_lvColumn;

	m_lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;   
	m_lvColumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	m_lvColumn.pszText = sCol;
	m_lvColumn.iSubItem = 0;
	m_lvColumn.cx = nListSize;
	ctrList->InsertColumn(0, &m_lvColumn);

	m_nMaxList = nMaxList;
	m_ctrListLog = ctrList;
	m_bListCreate = TRUE;
	m_bListTimeView = isListTimeView;

	return m_bListCreate;
}

BOOL CLogFile::ListAllDelete()
{
	if(!m_bListCreate || (m_ctrListLog == NULL))
	{
		AfxMessageBox(_T("ListCtrl Create Fail !!"));
		return FALSE;
	}
	BOOL rslt = m_ctrListLog->DeleteAllItems();
	return rslt;
}