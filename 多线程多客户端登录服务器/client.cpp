// server.cpp : 定义控制台应用程序的入口点。

#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")//表示链接到ws2_32.lib这个库，ws2_32是winsock2的库文件 这里是库文件！！！
//#pragma comment(compiler)
//winsock2相当于连接系统和我使用的软件之间交流的一个接口
//CSocket在WSock32.dll中实现
//Socket API在ws2_32.dll中实现
//这两个dll是两个不同层次的接口，调用关系如下：
//网络应该程序>>CSocket(Wsock32.dll)>>Socket API(ws2_32)>>网络传输服务进程
//dll（dynamic link library）动态链接库文件（应用程序扩展）在Windows中，
//许多应用程序不是一个完整的可执行文件，会被分割成一些相对独立的动态链接库（dll），
//当执行某一个程序时，相应的dll就会被调用

const char default_port[] = "5000";//默认端口
								   //const int recieve_buf_size = 256;//默认缓冲区大小
const int send_buf_size = 1024;//默认发送信息缓冲区大小
const int ip_buf_size = 129;

int main()
{
	int i_result = 0;

	WORD Version = MAKEWORD(2, 2);//MAKEWORD(2,2)使用Winsock2.2版本

								  //Windows Socket Asynchronous Data结构体struct，调用WSAStartup()函数后返回的Windows Socket数据，
								  //包含Winsock.dll执行的结果
	WSADATA wsadata;

	//WSAStartup()函数向操作系统说明我要使用哪个库文件，让该库文件与当前的应用程序绑定，
	//从而可以调用该版本的socket的各种函数
	i_result = WSAStartup(Version, &wsadata);//wsadata包含Windows socket执行的信息
											 //加载winsock库并确定winsock版本，系统会把数据填入wsaData中  

	if (i_result != 0)//WSAStartup()函数返回一个int型指，通过这个值检查初始化是否成功
	{
		cout << "WSAStartup() function failed with error :" << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 0;
	}

	addrinfo *result;//存储地址信息的链表
	addrinfo hints;//是对result的暗示，希望getaddrinfo()函数返回result的值是怎么样的，就给hints设置成什么样的

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = AI_PASSIVE;//AI_PASSIVE当此标志置位时，表示调用者将在bind()函数调用中使用返回的地址结构。当此标志不置位时，表示将在connect()函数调用中使用
	hints.ai_family = AF_INET;//IPv4协议栈
	hints.ai_socktype = SOCK_STREAM;//socket类型：流式套接字
	hints.ai_protocol = IPPROTO_TCP;//ai_protocol指定协议类型,可取的值，取决于ai_address和ai_socktype的值

									//SOCKET server = INVALID_SOCKET;
	SOCKET client = INVALID_SOCKET;

	//getaddrinfo()返回地址信息，地址信息存放在addrinfo结构体result中 
	//原型：int i_result=getaddrinfo(*hostname,*service,struct addrinfo *hints, struct addrinfo **result );
	//service：服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等
	i_result = getaddrinfo("127.0.0.2", default_port, &hints, &result);
	//当hostname主机名为NULL，AI_PASSIVE置位，则返回的地址将是通配地址，通配地址即全0，代表任意地址

	if (i_result != 0)
	{
		cout << "getaddrinfo() function failed with error :" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 0;
	}
	//原型：int socket( int af, int type, int protocol)根据主机地址信息result创建一个服务器端的套接字SOCKET server
	//不成功返回INVALID_SOCKET
	client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//原型：SOCKET server=socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (client == INVALID_SOCKET)
	{
		cout << "socket() function failed with error : " << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 0;
	}

	i_result = connect(client, result->ai_addr, result->ai_addrlen);
	//客户端使用，相当于服务器端的bind()函数
	//用于建立与指定socket的连接
	//函数原型: int i_reault=connect(SOCKET client,result->ai_addr, result->ai_addrlen);
	if (i_result == INVALID_SOCKET)
	{
		cout << "connect() function failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 0;
	}

	cout << "连接成功..." << endl;
	freeaddrinfo(result);

	char send_buf[send_buf_size];
	int recieve_result = 0;

	do
	{
		memset(send_buf, 0, sizeof(send_buf));//缓存区间先置0

		cout << "输入想发送的信息：";
		cin.getline(send_buf, send_buf_size);
		i_result = send(client, send_buf, (strlen(send_buf)), 0);
		//函数原型：int i_result=send( SOCKET client, send_buf, int len, int flags);

		if (i_result == SOCKET_ERROR)
		{
			cout << "send() function failed with error :" << WSAGetLastError() << endl;
			closesocket(client);
			WSACleanup();
			system("pause");
			return 0;
		}

		cout << "此次共发送信息字节数：" << i_result << endl;

		//接收服务器返回的信息
		recieve_result = recv(client, send_buf, send_buf_size, 0);
		//函数原型:int recieve_result=recv( SOCKET client, recieve_reuslt, int len, int flags)
		//本函数用于已连接的数据报或流式套接口进行数据的接收

		if (recieve_result > 0)
		{
			cout << "服务器返回信息的字节数为：" << send_buf << endl;
		}
		else if (recieve_result == 0)
		{
			cout << "连接关闭..." << endl;
		}
		else
		{
			cout << "recv() function failed with error: " << WSAGetLastError() << endl;
			closesocket(client);
			WSACleanup();
			system("pause");
			return 0;
		}
	} while (recieve_result > 0);

	i_result = shutdown(client, SD_SEND);
	//函数原型：int i_result=shutdown( SOCKET client, int how);
	//how：标志，用于描述禁止哪些操作
	//取值为：SD_RECEIVE、SD_SEND、SD_BOTH(SD=shutdown)
	if (i_result != 0)
	{
		cout << "shutdown() function failed with error: " << WSAGetLastError() << endl;
		closesocket(client);
		WSACleanup();
		system("pause");
		return 0;
	}

	closesocket(client);
	WSACleanup();
	cout << "关闭socket..." << endl;
	system("pause");
	return 0;
}
