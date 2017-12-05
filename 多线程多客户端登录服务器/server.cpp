
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
	addr_server.sin_port = ::htons(default_port);
	//sin_port 通信端口号必须是网络字节数据格式host to network short
	//结构体SOCKADDR_IN中还有一个struct in_addr 并且默认有一个结构体对象为：sin_addr
	//struct in_addr结构体中，使用联合union，并且默认创建一个union为S_un
	//S_un有三种方式来保存IP地址信息；
	//关于IP地址，这是一个4字节的无符号整型，此结构体也就对应了三种保存方式：
	//1.S_un_b单字节保存；
	//2.S_un_w，双字节保存；
	//3.S_addr，4字节保存；
	//常用S_addr，也就是4字节直接保存IP地址信息
	addr_server.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(ip_buf, 0, ip_buf_size);

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
	::closesocket(client);
	::WSACleanup();
	cout << "关闭套接字..." << endl;
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter);
//DWORD：double word，word为2字节，则DWORD为4字节
//创建线程函数CreateClientThread
//LPVOID：long point void 无类型指针
//lpParameter：

void Server::WaitForClient()
{
	while (true)
	{
		client = ::accept(server, (SOCKADDR *)&addr_client, &addr_len);
		if (client == INVALID_SOCKET)
		{
			cout << "accept() function failed with error: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		inet_ntop(addr_client.sin_family, &addr_client, ip_buf, ip_buf_size);
		//::InetNtop(addr_client.sin_family, &addr_client, PWSTR(ip_buf), ip_buf_size);
		cout << "一个新的用户已经连接...IP地址为: " << endl << ip_buf << endl << "端口号为: " << endl <<
			::ntohs(addr_client.sin_port) << endl;

		h_thread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)client, 0, nullptr);
		if (h_thread == NULL)
		{
			cout << "CreateThread() function failed with error: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pasue");
			exit(1);
		}
		::CloseHandle(h_thread);
	}
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET client = (SOCKET)lpParameter;
	char message_buf[message_buf_size];
	int result = 0;
	int send_result = 0;
	do
	{
		memset(message_buf, 0, message_buf_size);
		result = ::recv(client, message_buf, message_buf_size, 0);
		if (result > 0)
		{
			if (strcmp(message_buf, "exit") == 0)
			{
				cout << "用户请求关闭连接..." << endl;
				break;
			}
			cout << "收到消息：" << message_buf << endl;
			send_result = ::send(client, message_buf, message_buf_size, 0);
			if (send_result == SOCKET_ERROR)
			{
				cout << "send() function failed with error: " << ::WSAGetLastError() << endl;
				::WSACleanup();
				::closesocket(client);
				system("pause");
				return 1;
			}
		}
		else if (result == 0)
		{
			cout << "连接关闭..." << endl;
		}
		else
		{
			cout << "recv() function failed with error: " << ::WSAGetLastError() << endl;
			::closesocket(client);
			::WSACleanup();
			system("pause");
			return 1;
		}
	} while (result > 0);
	result = ::shutdown(client, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		cout << "shutdown() function failed with error: " << ::WSAGetLastError() << endl;
		::closesocket(client);
		::WSACleanup();
		system("pause");
		return 1;
	}
	return 0;
}

