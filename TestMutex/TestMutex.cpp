// TestMutex.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TestMutex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

 //���ͷ�����Ȩ,Ȼ���˳��߳�,�����ȴ���������wait�̻߳�
//�յ�һ��WAIT_ABANDONED�ź� [LCM 11/19/2012 ]
unsigned __stdcall CreateMutexthread(void *)
{
	HANDLE hMutex = CreateMutex(NULL,FALSE,L"AA11");
	if(hMutex != 0)
	{
		//MessageBox(NULL,L"aa11 ok",L"test",MB_OK);
		cout<<"create mutex ok"<<endl;
		DWORD dwRet = WaitForSingleObject(hMutex,5);
		cout<<"CreateMutexthread wait :"<<dwRet<<endl;		
	}
	return 1;
}
unsigned __stdcall WaitThreadA(void *)
{
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,L"AA11");
	if(hMutex != 0)
	{
		while (true)
		{
			DWORD dwRet = WaitForSingleObject(hMutex,5);
			switch (dwRet)
			{
			case WAIT_OBJECT_0:
				cout<< "WaitThreadA wait aa11 ok"<<endl;
				if(ReleaseMutex(hMutex))
					cout<<"WaitThreadA  "<<"release mutex  ok"<<endl;
				else
					cout<<"WaitThreadA"<< "release mutex fail"<<endl;
				Sleep(2000);
				break;
			case WAIT_ABANDONED:
				// �յ������Ϣ��ʾ���߳�û���ͷ��������������������ȴ����´ο����յ�WAIT_OBJECT_0����
				// ������ﲻ�ͷţ�����Ȩ�ͻ�һֱ������̣߳���[LCM 11/19/2012 ]
				cout<< "WaitThreadA wait aa11 WAIT_ABANDONED"<<endl;
				ReleaseMutex(hMutex);
				break;
			case WAIT_TIMEOUT:
				cout<< "WaitThreadA wait aa11 WAIT_TIMEOUT"<<endl;
				break;
			default:
				cout<< "WaitThreadA wait aa11 other:"<<dwRet<<endl;
				break;
			}
			
			Sleep(3000);
		}
	}
	return 1;
}



unsigned __stdcall WaitThreadB(void *)
{
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,L"AA11");
	if(hMutex != 0)
	{
		//MessageBox(NULL,L"aa11 ok",L"test",MB_OK);
		cout<<"WaitThreadB open mutex ok"<<endl;
		while(true)
		{
			DWORD dwRet = WaitForSingleObject(hMutex,5);
			cout<<"WaitThreadB wait :"<<dwRet<<endl;
			ReleaseMutex(hMutex);
			Sleep(2000);
			//���ͷ�����Ȩ,Ȼ���˳��߳�,�����ȴ���������wait�̻߳��յ�һ��WAIT_ABANDONED�ź�
		}
	}
	return 1;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		
		HANDLE hthread1 = (HANDLE)_beginthreadex(NULL,0,CreateMutexthread,0,0,0);
		WaitForSingleObject(hthread1,INFINITE);
		cout<<"CreateMutexthread finish!"<<endl;
 
 		HANDLE hthread3 = (HANDLE)_beginthreadex(NULL,0,WaitThreadB,0,0,0);
 		HANDLE hthread2 = (HANDLE)_beginthreadex(NULL,0,WaitThreadA,0,0,0);
 		WaitForSingleObject(hthread2,INFINITE);
 		cout<<"WaitThreadA end....."<<endl;
 		WaitForSingleObject(hthread3,INFINITE);
 		cout<<"WaitThreadB end....."<<endl;

	}

	return nRetCode;
}
