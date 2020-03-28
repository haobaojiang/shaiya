#include "stdafx.h"
#include "DataBase.h"


CDataBase::CDataBase(void)
{

	m_dwIndex=0;
}


CDataBase::~CDataBase(void)
{
	clear();
}



std::wstring CDataBase::ExeSqlByCommand(std::wstring szSql, std::wstring szFieldName)
{
	//1.执行语句
//	_RecordsetPtr  pRecord;
	_CommandPtr pCommand;
	_variant_t  AffectedLines;
	pCommand.CreateInstance(__uuidof(Command));
	pCommand->CommandText = _bstr_t(szSql.c_str());
	pCommand->ActiveConnection = m_pConnect;
	m_pRecordset  = pCommand->Execute(&AffectedLines, NULL, adCmdText);
	//2.如果有记录集数据就解析一下
	if (m_pRecordset->GetState())
	{
		//2.解析数据
		_variant_t stcSign = m_pRecordset->GetadoEOF();//获取游标,看看是否结速
		while (stcSign.boolVal != -1)
		{
			_variant_t varTemp = m_pRecordset->GetCollect(_bstr_t(szFieldName.c_str()));//获取字段值
			if (varTemp.vt != VT_NULL)
			{
				std::wstring szTemp = (WCHAR*)_bstr_t(varTemp);
				return szTemp;
			}
			m_pRecordset->MoveNext();
			stcSign = m_pRecordset->GetadoEOF();
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
	   HRESULT hr1 = m_pConnect.CreateInstance(L"ADODB.Connection");
	   if (FAILED(hr1))
	   {
		   return FALSE;
	   }
   }
	catch (_com_error & e)
	{
		return FALSE;
	}
	WCHAR szLink[MAX_PATH] = { 0 };
	swprintf_s(szLink, L"Provider=SQLOLEDB;DataTypeCompatibility=80;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=%s;Data Source=(Local)", DbName);

	HRESULT hResult=m_pConnect->Open(_bstr_t(szLink), _bstr_t(L""), _bstr_t(L""), adOpenUnspecified);

	m_pCommand.CreateInstance(__uuidof(Command));
	m_pCommand->ActiveConnection = m_pConnect;


	if (FAILED(hResult))
		return FALSE;
	else
		return TRUE;
}


BOOL CDataBase::LinkDataBase(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw)
{
	try
	{
		m_pConnect.CreateInstance(L"ADODB.Connection");
		WCHAR szLink[MAX_PATH] = { 0 };
		swprintf_s(szLink, L"Provider=SQLOLEDB;Server=%s;Database=%s;Uid=%s;Pwd=%s", ip, DbName, UserName, Pw);

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




BOOL CDataBase::ExeSqlByCon(std::wstring szSql)
{
	try{
		m_pConnect->Execute(_bstr_t(szSql.c_str()),NULL,1);//执行SQL语句
	}
	catch(_com_error & e)
	{
		return FALSE;
	}
	return TRUE;
}



BOOL CDataBase::ExeSqlByCommand(std::wstring szSql)
{
	try{
		//1.执行语句

		m_pCommand->CommandText = _bstr_t(szSql.c_str());
		m_pRecordset = m_pCommand->Execute(NULL, NULL, adCmdText);
		//如果有记录集数据就解析一下
		if (m_pRecordset->GetState())
		{
			ExpainRecord(m_pRecordset);
		}
		m_pRecordset->Close();

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
	m_vecDATABASE.clear();
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
	while (stcSign.boolVal != -1)
	{
		//通过自身的列名获取相应的这一行的数据

		for (long i = 0; i < nCount; i++)
		{
			_variant_t varTemp = pRecord->GetCollect(_bstr_t(m_vecDATABASE[i].szName.c_str()));
			if (varTemp.vt != VT_NULL)
			{
				std::wstring szTemp = (WCHAR*)_bstr_t(varTemp);
				m_vecDATABASE[i].vecValue.push_back(szTemp);
			}
		}
		pRecord->MoveNext();
		m_nRecordCount++;
		stcSign = pRecord->GetadoEOF();
	}
//	m_nRecordCount--;
	return TRUE;
}


std::wstring CDataBase::GetValueByField(std::wstring szField)	//获取记录集里的内容
{
	for (DWORD i = 0; i < m_vecDATABASE.size(); i++)
	{
		if (szField.compare(m_vecDATABASE[i].szName) == 0)
		{
			return m_vecDATABASE[i].vecValue[m_dwIndex];//返回指定字段当前游标下的内容
		}
	}
	return L"";
}