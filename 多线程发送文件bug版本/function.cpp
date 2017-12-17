#pragma warning(disable:4996)
#include"afxmt.h"
#include<afx.h>
#include"server.h"
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;
#pragma comment (lib,"Ws2_32.lib")

#define ip_buf_size 129
#define default_port 5000
#define message_buf_size 1024

Server::Server()
{
	cout << "初始化服务器端..." << endl;

	version = MAKEWORD(2, 2);
	addr_len = sizeof(SOCKADDR_IN);

	addr_server.sin_family = AF_INET;
	//结构体SOCKADDR_IN中的sin_port必须要采用网络数据格式，普通十进制数字需要先进行转换
	addr_server.sin_port = ::htons(5000);
	//sin_port 通信端口号必须是网络字节数据格式host to network short
	//结构体SOCKADDR_IN中还有一个struct in_addr 并且默认有一个结构体对象为：sin_addr
	//struct in_addr结构体中，使用联合union，并且默认创建一个union为S_un
	//S_un有三种方式来保存IP地址信息；
	//关于IP地址，这是一个4字节的无符号整型，此结构体也就对应了三种保存方式：
	//1.S_un_b单字节保存；
	//2.S_un_w，双字节保存；
	//3.S_addr，4字节保存；
	//常用S_addr，也就是4字节直接保存IP地址信息
	addr_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.2");
	//memset(ip_buf, 0, ip_buf_size);

	result = ::WSAStartup(version, &wsadata);
	if (result != 0)
	{
		cout << "WSAStartup() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "Windows Socket Application Programming Interface 初始化成功..." << endl;

	server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		cout << "socket() function failed wit error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "服务器端套接字创建成功..." << endl;

	result = ::bind(server, (SOCKADDR *)&addr_server, addr_len);
	if (result != 0)
	{
		cout << "bind() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "套接字和套接字地址信息绑定成功..." << endl;

	result = ::listen(server, SOMAXCONN);
	if (result != 0)
	{
		cout << "listen() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "开始监听..." << endl;
	cout << "服务器端创建完成..." << endl;
}
Server::~Server()
{
	::closesocket(server);
	//::closesocket(client);
	::WSACleanup();
	cout << "关闭套接字..." << endl;
}

DWORD GetInfoProc(SOCKET client)
{
	CFile file;
	int send_result;
	if (file.Open(L"‪E:/数据结构与算法.pdf", CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary,NULL))
	{
		int FileLen = file.GetLength();
		send_result = send(client, (char *)&FileLen, sizeof(FileLen), 0);
		if (send_result == SOCKET_ERROR)
		{
			cout << "send() function failed with error: " << WSAGetLastError() << endl;
			WSACleanup();
			closesocket(client);
			system("pause");
			return 1;
		}
		cout << "文件长度为：" << FileLen << endl;
	}
	else
	{
		cout << "open() function failed"<< endl;
		system("pause");
		return 0;
	}
	return 0;
}

DWORD WINAPI GetFileProc(LPVOID lparam)
{
	CRITICAL_SECTION CS;
	FILEINFO TempFileInfo;
	memset((char *)&TempFileInfo, 0, sizeof(TempFileInfo));

	InitializeCriticalSection(&CS);
	EnterCriticalSection(&CS);
	int FileLen = TempFileInfo.FileLen;
	int Seek = TempFileInfo.seek;
	SOCKET client = TempFileInfo.sockid;
	LeaveCriticalSection(&CS);
	DeleteCriticalSection(&CS);

	CFile file;
	if (file.Open(L"E:/壁纸/007.jpg", CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary,NULL))
	{
		file.Seek(Seek, CFile::begin);
		char *date = new char[FileLen];
		int nLeft = FileLen;
		int idx = 0;
		file.Read(date, FileLen);
		while (nLeft)
		{
			int ret = send(client, &date[idx], nLeft, 0);
			if (ret == SOCKET_ERROR)
			{
				cout << "send() function failed with error: " << WSAGetLastError() << endl;
				system("pause");
				return 0;
			}
			nLeft -= ret;
			idx += ret;
		}
		file.Close();
		delete[] date;
	}
	else
	{
		cout << "打开文件错误" << endl;
	}
	closesocket(client);
	return 0;
}

void Server::WaitForClient()
{
	while (true)
	{
		if (client = accept(server, (SOCKADDR *)&addr_client, &addr_len))
		{
			memset((char*)&recieve_fileinfo, 0, sizeof(recieve_fileinfo));
			if (recv(client, (char *)&recieve_fileinfo, sizeof(recieve_fileinfo), 0)==SOCKET_ERROR)
			{
				cout << "recv() function failed with error: " << WSAGetLastError() << endl;
				WSACleanup();
				system("pause");
				exit(1);
			}
			else
			{
				InitializeCriticalSection(&CS);
				EnterCriticalSection(&CS);
				memcpy((char *)&TempFileInfo, (char*)&recieve_fileinfo, sizeof(recieve_fileinfo));
				switch (TempFileInfo.CMD)
				{
				case 1:
					GetInfoProc(client);
					break;
				case 2:
					TempFileInfo.sockid = client;
					CreateThread(NULL, NULL, GetFileProc, NULL, NULL, NULL);
					break;
				}
				LeaveCriticalSection(&CS);
				DeleteCriticalSection(&CS);
			}
		}
	}
}


