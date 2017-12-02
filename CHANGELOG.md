# 代码完成情况

单线程部分暂时应该没什么问题了，可以正常运行

开始写多线程的部分

时间：2017.11.19

---

# 修改了部分代码，现在可以确认是完全正确了

在Windows cmd窗口下，前后分别运行server.exe client.exe 在client里输入内容可以传输到server里

输入exit停止传输

完成了一个单线程客户端登录服务器的程序

![Image text](https://github.com/shangli-cuc/picture/blob/master/%E5%8D%95%E7%BA%BF%E7%A8%8B%E7%99%BB%E5%BD%95%E6%9C%8D%E5%8A%A1%E5%99%A8Windows-Socket%E8%BF%90%E8%A1%8C.png)

时间：2017.11.24

---

# 完成了多线程部分的代码

在Windows cmd窗口下，前后分别运行server.exe client.exe 和之前的单线程部分一样，但是多线程支持多个用户登录服务器，发送消息

同样，输入exit停止传输

![Image text](https://github.com/shangli-cuc/picture/blob/master/%E5%A4%9A%E7%BA%BF%E7%A8%8B%E7%99%BB%E5%BD%95%E6%9C%8D%E5%8A%A1%E5%99%A8Windows-Socket%E8%BF%90%E8%A1%8C.png)

现在发现了2个bug

1、从client端发送很多内容的时候，server端可以收到，但是返回给client收到的内容时会发生延迟之类的问题
2、client端突然掉线（没有通过输入exit的方式停止传输，采取关闭cmd窗口的方式）时，哪怕还有其他client在线，整个server也会停止

时间：2017.11.29

# debug上述bug的情况说明

之前发现发送给server端的信息，server本身可以收到，但是不能返回client，最后发现是定义的ip_buf_size和send_buf_size长度忘记改了

改完之后，debug成功

![Image text](https://github.com/shangli-cuc/picture/blob/master/%E5%A4%9A%E7%BA%BF%E7%A8%8B%E7%99%BB%E5%BD%95%E6%9C%8D%E5%8A%A1%E5%99%A8debug.png)

时间：2017.12.02
