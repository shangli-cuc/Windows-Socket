//头文件server.h


#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<WS2tcpip.h>
using namespace std;

#pragma comment (lib,"Ws2_32.lib")

#define ip_buf_size 129
#define default_port 5000
#define message_buf_size 1024
struct FILEINFO
{
	char Name[100];//文件名称
	int FileLen;//文件长度
	int CMD;//操作指令
	int seek;//线程开始位置
	SOCKET sockid;
};
class Server
{
public:
	Server();//构造函数 创建对象
	~Server();//析构函数 销毁对象
	Server(const Server &) = delete;//将该函数定义成已删除的函数，任何试图调用它的行为将产生编译期错误
	Server & operator=(const Server &) = delete;
	void WaitForClient();
private:
	WORD version;
	WSADATA wsadata;
	SOCKET server;
	SOCKET client;
	FILEINFO recieve_fileinfo;
	FILEINFO TempFileInfo;
	//FILEINFO FI;
	CRITICAL_SECTION CS;
	//HANDLE Thread[10];

	//套接字地址信息
	SOCKADDR_IN addr_server;
	SOCKADDR_IN addr_client;
	int result;
	int addr_len;
	char ip_buf[ip_buf_size];
};

