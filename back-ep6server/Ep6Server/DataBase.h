/***************************************使用方法*****************************************

//或者是直接用m_objDataBase.LinkDataBase();这种是属于本地系统账号连接

BOOL bSuccess;
bSuccess=m_objDataBase.LinkDataBase(L"192.168.1.3",L"ps_userdata",L"sa",L"fishleong");
if(bSuccess)
{
	AfxMessageBox(L"数据库连接成功");
}
else
{
	AfxMessageBox(L"数据库连接失败");
}
}*


//1.获取SQL语句
vector<_DATABASEINFO> vecDBInfo;
CString szSql;
GetDlgItemText(IDC_EDIT1,szSql);             
//2.执行SQL语句
m_objDataBase.ExeSqlByCommand(szSql,vecDBInfo);   //执行的语句
//3.输出SQL语句得出的信息
m_List_Info.ResetContent();
for(int i=0;i<vecDBInfo.size();i++)
{
for(int j=0;j<vecDBInfo[i].vecValue.size();j++)
{
m_List_Info.AddString(vecDBInfo[i].vecValue[j]);
}	
}
***********************************************************************************************/

//#define ADO2_OLD 0x1
#pragma once


#if defined(ADO2_OLD)
#import "msado20.tlb" no_namespace rename("EOF", "adoEOF") 
#else   
#import "msado15.dll" no_namespace rename("EOF", "adoEOF") 
#endif


#include <vector>
using std::vector;


typedef struct _DATABASEINFO
{
	CString szName;          //字段名
	vector<CString> vecValue;//字段值

}DATABASEINFO,*PDATABASEINFO;

typedef struct _StroeParam
{
	
	ParameterDirectionEnum dwType; //类型 adParamInput 或是adParamOutput
	DataTypeEnum dwDataType;       //adVarChar 或是 adInteger
	char  strName[MAX_PATH];       //参数的名字
	char  strValue[MAX_PATH];      //参数的值
	char  strResult[MAX_PATH];     //参数的返回结果,只有adParamOutput才有
}StroeParam, *PStroeParam;


class CDataBase
{
public:
	CDataBase(void);
	~CDataBase(void);
	BOOL    LinkDataBase(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw);//远程连接
	BOOL    LinkDataBase2012(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw);

	BOOL    LinkDataBase(const WCHAR* DbName);                                  //本地系统账号连接
	BOOL    ExeSqlByCon(CString szSql);                                         //执行sql语句,不返回记录集
	CString ExeSqlByCommand(CString szSql, CString szFieldName);                //执语语句,获取第二个参数的返回值,一般是返回一条记录的值
	BOOL    ExeSqlByCommand(CString szSql);                                     //执语句语,解析记录集到成员函数
	BOOL    ExpainRecord(_RecordsetPtr &pRecord);                               //解析记录集
	CString GetValueByField(CString szField);	                                //获取记录集里的内容

	void clear()

	{
		m_vecDATABASE.clear();
		m_dwIndex=0;
		m_nRecordCount = 0;
	}
	//4.设置游标
	void SetSeek(DWORD dwSeek)
	{
		m_dwIndex=dwSeek;
	}
	//5.设置游标
	void MoveNext()
	{
		m_dwIndex++;
	}
	//6.检测是否还有数据了
	BOOL IsEmpty()
	{
		if(m_dwIndex>=m_vecDATABASE[0].vecValue.size()) 
		{
			return TRUE;
		}
		return FALSE;
	}
	//7.数据库
	CString ExecProc(std::string strProduteName, std::string tstr)
	{
		_CommandPtr cmd;
		cmd.CreateInstance("ADODB.Command");

		//1.参数1
		_ParameterPtr pParamRk;
		pParamRk.CreateInstance("ADODB.Parameter");
		pParamRk->Name = "charname";	//存储过程的参数1
		pParamRk->Type = adChar;	    //整型
		pParamRk->Size = 20;			      //
		pParamRk->Direction = adParamInput;//表明是输入参数
		pParamRk->Value = _variant_t(tstr.c_str());
		cmd->Parameters->Append(pParamRk);

		//2.参数2:返回值
		_ParameterPtr pParamOk;
		pParamOk.CreateInstance("ADODB.Parameter");
		pParamOk->Name = "Availiable";		//参数2名称
		pParamOk->Type = adInteger; //字符串
		pParamOk->Size = 2;			//
		pParamOk->Direction = adParamOutput;	//声明是输出参数
		cmd->Parameters->Append(pParamOk);

		//3.执行存储过程
		cmd->ActiveConnection = m_pConnect;
		cmd->CommandText = strProduteName.c_str();	//存储过程的名字
		cmd->CommandType = adCmdStoredProc;//表示为存储过程adCmdStoredProc
		cmd->Execute(NULL, NULL, adCmdStoredProc);

		//4.返回执行结果
		return (char*)_bstr_t(pParamOk->Value);
	}


	void ExecProc(std::string strProduteName, DWORD dwCount,PStroeParam pStore)
	{
		//1.初始化
		_CommandPtr cmd;
		cmd.CreateInstance("ADODB.Command");

		//2.创建参数
		_ParameterPtr* pParameter = new _ParameterPtr[dwCount];
		for (DWORD i = 0; i < dwCount; i++)
		{
			_ParameterPtr pParamRk = pParameter[i];
			pParamRk.CreateInstance("ADODB.Parameter");
			pParameter[i] = pParamRk;
			pParamRk->Name = pStore[i].strName;	      //存储过程的参数名字
			pParamRk->Type = pStore[i].dwDataType;    //传参的数据类型
			if (pParamRk->Type == adVarChar)          //长度,反正除了varchar一律为4,也就是整形
			{
				pParamRk->Size = 20;
			}
			else
			{
				pParamRk->Size = 4;
			}

			pParamRk->Direction = pStore[i].dwType; //是输入参数还是返回参数
			if (pParamRk->Direction == adParamInput)//只有输入参数才需要值,其它不需要复制
			{
				pParamRk->Value = _variant_t(pStore[i].strValue);
			}

			cmd->Parameters->Append(pParamRk);
		}

		//3.执行存储过程
		cmd->ActiveConnection = m_pConnect;
		cmd->CommandText = strProduteName.c_str();	//存储过程的名字
		cmd->CommandType = adCmdStoredProc;         //表示为存储过程adCmdStoredProc
		cmd->Execute(NULL, NULL, adCmdStoredProc);

		//4.返回执行结果
		for (DWORD i = 0; i < dwCount; i++)
		{
			_ParameterPtr pParamRk = pParameter[i];
			strcpy_s(pStore[i].strResult, MAX_PATH, _bstr_t(pParamRk->Value));
		}
	}

public:
	_ConnectionPtr        m_pConnect;   //连接对象
	vector<_DATABASEINFO> m_vecDATABASE;//记录集
	DWORD                 m_dwIndex;    //游标  默认为0
	DWORD                 m_nRecordCount;
	_CommandPtr m_pCommand;
	_RecordsetPtr m_pRecordset;

};

