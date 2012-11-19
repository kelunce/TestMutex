// TestMutex.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TestMutex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

 //不释放所有权,然后退出线程,其他等待这个对象的wait线程会
//收到一个WAIT_ABANDONED信号 [LCM 11/19/2012 ]
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
				// 收到这个消息表示有线程没有释放这个互斥量，如果继续等待，下次可以收到WAIT_OBJECT_0！！
				// 如果这里不释放，所有权就会一直在这个线程！！[LCM 11/19/2012 ]
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
			//不释放所有权,然后退出线程,其他等待这个对象的wait线程会收到一个WAIT_ABANDONED信号
		}
	}
	return 1;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		
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
