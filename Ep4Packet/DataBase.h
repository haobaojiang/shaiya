/***************************************ʹ�÷���*****************************************

//������ֱ����m_objDataBase.LinkDataBase();���������ڱ���ϵͳ�˺�����

BOOL bSuccess;
bSuccess=m_objDataBase.LinkDataBase(L"192.168.1.3",L"ps_userdata",L"sa",L"fishleong");
if(bSuccess)
{
	AfxMessageBox(L"���ݿ����ӳɹ�");
}
else
{
	AfxMessageBox(L"���ݿ�����ʧ��");
}
}*


//1.��ȡSQL���
vector<_DATABASEINFO> vecDBInfo;
CString szSql;
GetDlgItemText(IDC_EDIT1,szSql);             
//2.ִ��SQL���
m_objDataBase.ExeSqlByCommand(szSql,vecDBInfo);   //ִ�е����
//3.���SQL���ó�����Ϣ
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
	CString szName;          //�ֶ���
	vector<CString> vecValue;//�ֶ�ֵ

}DATABASEINFO,*PDATABASEINFO;

typedef struct _StroeParam
{
	
	ParameterDirectionEnum dwType; //���� adParamInput ����adParamOutput
	DataTypeEnum dwDataType;       //adVarChar ���� adInteger
	char  strName[MAX_PATH];       //����������
	char  strValue[MAX_PATH];      //������ֵ
	char  strResult[MAX_PATH];     //�����ķ��ؽ��,ֻ��adParamOutput����
}StroeParam, *PStroeParam;


class CDataBase
{
public:
	CDataBase(void);
	~CDataBase(void);
	BOOL    LinkDataBase(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw);//Զ������
	BOOL    LinkDataBase2012(const WCHAR* ip, const WCHAR* DbName, const WCHAR* UserName, const WCHAR* Pw);

	BOOL    LinkDataBase(const WCHAR* DbName);                                  //����ϵͳ�˺�����
	BOOL    ExeSqlByCon(CString szSql);                                         //ִ��sql���,�����ؼ�¼��
	CString ExeSqlByCommand(CString szSql, CString szFieldName);                //ִ�����,��ȡ�ڶ��������ķ���ֵ,һ���Ƿ���һ����¼��ֵ
	BOOL    ExeSqlByCommand(CString szSql);                                     //ִ�����,������¼������Ա����
	BOOL    ExpainRecord(_RecordsetPtr &pRecord);                               //������¼��
	CString GetValueByField(CString szField);	                                //��ȡ��¼���������

	void clear()

	{
		m_vecDATABASE.clear();
		m_dwIndex=0;
		m_nRecordCount = 0;
	}
	//4.�����α�
	void SetSeek(DWORD dwSeek)
	{
		m_dwIndex=dwSeek;
	}
	//5.�����α�
	void MoveNext()
	{
		m_dwIndex++;
	}
	//6.����Ƿ���������
	BOOL IsEmpty()
	{
		if(m_dwIndex>=m_vecDATABASE[0].vecValue.size()) 
		{
			return TRUE;
		}
		return FALSE;
	}
	//7.���ݿ�
	CString ExecProc(std::string strProduteName, std::string tstr)
	{
		_CommandPtr cmd;
		cmd.CreateInstance("ADODB.Command");

		//1.����1
		_ParameterPtr pParamRk;
		pParamRk.CreateInstance("ADODB.Parameter");
		pParamRk->Name = "charname";	//�洢���̵Ĳ���1
		pParamRk->Type = adChar;	    //����
		pParamRk->Size = 20;			      //
		pParamRk->Direction = adParamInput;//�������������
		pParamRk->Value = _variant_t(tstr.c_str());
		cmd->Parameters->Append(pParamRk);

		//2.����2:����ֵ
		_ParameterPtr pParamOk;
		pParamOk.CreateInstance("ADODB.Parameter");
		pParamOk->Name = "Availiable";		//����2����
		pParamOk->Type = adInteger; //�ַ���
		pParamOk->Size = 2;			//
		pParamOk->Direction = adParamOutput;	//�������������
		cmd->Parameters->Append(pParamOk);

		//3.ִ�д洢����
		cmd->ActiveConnection = m_pConnect;
		cmd->CommandText = strProduteName.c_str();	//�洢���̵�����
		cmd->CommandType = adCmdStoredProc;//��ʾΪ�洢����adCmdStoredProc
		cmd->Execute(NULL, NULL, adCmdStoredProc);

		//4.����ִ�н��
		return (char*)_bstr_t(pParamOk->Value);
	}


	void ExecProc(std::string strProduteName, DWORD dwCount,PStroeParam pStore)
	{
		//1.��ʼ��
		_CommandPtr cmd;
		cmd.CreateInstance("ADODB.Command");

		//2.��������
		_ParameterPtr* pParameter = new _ParameterPtr[dwCount];
		for (DWORD i = 0; i < dwCount; i++)
		{
			_ParameterPtr pParamRk = pParameter[i];
			pParamRk.CreateInstance("ADODB.Parameter");
			pParameter[i] = pParamRk;
			pParamRk->Name = pStore[i].strName;	      //�洢���̵Ĳ�������
			pParamRk->Type = pStore[i].dwDataType;    //���ε���������
			if (pParamRk->Type == adVarChar)          //����,��������varcharһ��Ϊ4,Ҳ��������
			{
				pParamRk->Size = 20;
			}
			else
			{
				pParamRk->Size = 4;
			}

			pParamRk->Direction = pStore[i].dwType; //������������Ƿ��ز���
			if (pParamRk->Direction == adParamInput)//ֻ�������������Ҫֵ,��������Ҫ����
			{
				pParamRk->Value = _variant_t(pStore[i].strValue);
			}

			cmd->Parameters->Append(pParamRk);
		}

		//3.ִ�д洢����
		cmd->ActiveConnection = m_pConnect;
		cmd->CommandText = strProduteName.c_str();	//�洢���̵�����
		cmd->CommandType = adCmdStoredProc;         //��ʾΪ�洢����adCmdStoredProc
		cmd->Execute(NULL, NULL, adCmdStoredProc);

		//4.����ִ�н��
		for (DWORD i = 0; i < dwCount; i++)
		{
			_ParameterPtr pParamRk = pParameter[i];
			strcpy_s(pStore[i].strResult, MAX_PATH, _bstr_t(pParamRk->Value));
		}
	}

public:
	_ConnectionPtr        m_pConnect;   //���Ӷ���
	vector<_DATABASEINFO> m_vecDATABASE;//��¼��
	DWORD                 m_dwIndex;    //�α�  Ĭ��Ϊ0
	DWORD                 m_nRecordCount;
	_CommandPtr m_pCommand;
	_RecordsetPtr m_pRecordset;

};

