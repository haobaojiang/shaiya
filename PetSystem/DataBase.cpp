#include <afxwin.h>
#include "DataBase.h"


CDataBase::CDataBase(void)
{

	m_dwIndex=0;
}


CDataBase::~CDataBase(void)
{
}

CString CDataBase::ExeSqlByCommand(CString szSql, CString szFieldName)
{
	//1.执行语句
	_RecordsetPtr  pRecord;
	_CommandPtr pCommand;
	_variant_t  AffectedLines;
	pCommand.CreateInstance(__uuidof(Command));
	pCommand->CommandText = _bstr_t(szSql);
	pCommand->ActiveConnection = m_pConnect;
	pRecord = pCommand->Execute(&AffectedLines, NULL, adCmdText);
	//2.如果有记录集数据就解析一下
	if (pRecord->GetState())
	{
		//2.解析数据
		_variant_t stcSign = pRecord->GetadoEOF();//获取游标,看看是否结速
		while (stcSign.boolVal != -1)
		{
			_variant_t varTemp = pRecord->GetCollect(_bstr_t(szFieldName));//获取字段值
			if (varTemp.vt != VT_NULL)
			{
				CString szTemp = varTemp;
				return szTemp;
			}
			pRecord->MoveNext();
			stcSign = pRecord->GetadoEOF();
		}
		return L"0";
	}
	return L"0";
}



BOOL CDataBase::LinkDataBase(const WCHAR* DbName)
{
   ::CoInitialize(NULL);
   try
   {
	   HRESULT hr1 = m_pConnect.CreateInstance(_T("ADODB.Connection"));
	   if (FAILED(hr1))
	   {
		   return FALSE;
	   }
   }
	catch (_com_error & e)
	{
		OutputDebugString(L"失败1");
		return FALSE;
	}
	OutputDebugString(L"连接成功!");
	CString szLink;
	szLink.Format(L"Provider=SQLOLEDB;DataTypeCompatibility=80;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=%s;Data Source=(Local)", DbName);
	
	
	HRESULT hResult=m_pConnect->Open(_bstr_t(szLink), _bstr_t(L""), _bstr_t(L""), adOpenUnspecified);
	if (FAILED(hResult))
		return FALSE;
	else
		return TRUE;
}


BOOL CDataBase::LinkDataBase(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw)
{
	try
	{
		m_pConnect.CreateInstance(_T("ADODB.Connection"));
		CString szLink;
		szLink.Format(L"Provider=SQLOLEDB;Server=%s;Database=%s;Uid=%s;Pwd=%s", ip, DbName, UserName, Pw);
		HRESULT hResult=m_pConnect->Open(_bstr_t(szLink), _bstr_t(L""), _bstr_t(L""), adModeUnknown);

		if (FAILED(hResult))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
			
	}
	catch (_com_error & e)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDataBase::LinkDataBase2012(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw)
{
	try
	{
		m_pConnect.CreateInstance(_T("ADODB.Connection"));
		CString szLink;
		szLink.Format(L"Provider=sqlncli11;Server=%s;Database=%s;Uid=%s;Pwd=%s", ip, DbName, UserName, Pw);
		HRESULT hResult = m_pConnect->Open(_bstr_t(szLink), _bstr_t(L""), _bstr_t(L""), adModeUnknown);
		if (FAILED(hResult))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}

	}
	catch (_com_error & e)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CDataBase::ExeSqlByCon(CString szSql)
{
	try{
		m_pConnect->Execute(_bstr_t(szSql),NULL,1);//执行SQL语句
	}
	catch(_com_error & e)
	{
		return FALSE;
	}
	return TRUE;
}



BOOL CDataBase::ExeSqlByCommand(CString szSql)
{
	try{
		//1.执行语句
		_RecordsetPtr  pRecord;
		_CommandPtr pCommand;
		_variant_t  AffectedLines;
		pCommand.CreateInstance(__uuidof(Command));
		pCommand->CommandText = _bstr_t(szSql);
		pCommand->ActiveConnection = m_pConnect;
		pRecord = pCommand->Execute(&AffectedLines, NULL, adCmdText);
		//如果有记录集数据就解析一下
		if (pRecord->GetState()) ExpainRecord(pRecord);
	}
	catch (_com_error & e)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDataBase::ExpainRecord(_RecordsetPtr &pRecord)
{
	//1.解析列名
	Fields* pFields;
	pRecord->get_Fields(&pFields);
	long nCount = pFields->GetCount();   //记录多少字段
	for (long i = 0; i < nCount; i++)
	{
		DATABASEINFO stcTemp;
		BSTR         szName;
		pFields->Item[i]->get_Name(&szName);
		stcTemp.szName = szName;
		m_vecDATABASE.push_back(stcTemp);  //获取字段名
	}
	//2.解析数据
	_variant_t stcSign = pRecord->GetadoEOF();//获取游标,看看是否结速
	//m_nRecordCount = (DWORD)pRecord->RecordCount;
	while (stcSign.boolVal != -1)
	{
		//通过自身的列名获取相应的这一行的数据

		for (long i = 0; i < nCount; i++)
		{
			_variant_t varTemp = pRecord->GetCollect(_bstr_t(m_vecDATABASE[i].szName));
			if (varTemp.vt != VT_NULL)
			{
				CString    szTemp = varTemp;
				m_vecDATABASE[i].vecValue.push_back(szTemp);
			}
		}
		pRecord->MoveNext();
		m_nRecordCount++;
		stcSign = pRecord->GetadoEOF();
	}
	m_nRecordCount--;
	return TRUE;
}


CString CDataBase::GetValueByField(CString szField)	//获取记录集里的内容
{
	for (DWORD i = 0; i < m_vecDATABASE.size(); i++)
	{
		if (szField.Compare(m_vecDATABASE[i].szName) == 0)
		{
			return m_vecDATABASE[i].vecValue[m_dwIndex];//返回指定字段当前游标下的内容
		}
	}
	return L"";
}