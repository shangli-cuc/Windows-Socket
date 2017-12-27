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

---

# server端发多线程发送文件到client端（存在bug）

希望实现的功能是：server端打开一个文件读取它的长度，分成5个线程发送给client端，运行结果

![Image_text](https://github.com/shangli-cuc/picture/blob/master/%E5%A4%9A%E7%BA%BF%E7%A8%8B%E5%8F%91%E9%80%81%E6%96%87%E4%BB%B6bug%E7%89%88%E6%9C%AC.png)

初步想法是读取文件的函数用的有问题，这里用的是MFC的CFile，下一步准备实施fstream头文件使用的文件流函数

时间：2017.12.17

---

# 单线程发送文件（无bug，练手）

实现功能：server端发送较小文件到client端，没试过大文件，但是应该也可以发，可能会很慢，而且真的太大应该会把内存填满，出现溢出的问题。文件格式，试过了docx、txt，rar、zip都可以，但是PDF暂时不行，可能是格式限制。多线程发送文件的bug还没修改过来，先用这个试试发送文件的功能，就不上效果图了，因为输出到cmd窗口会出现乱码，看了也没用

时间：2017.12.20

---

# 多线程发送文件

实现功能：试了试txt、word文件都可以实现发送，但是视频死活不可以，好像有文件错位，这是个简版的，把报错语句都删掉了，看起来比较舒服，暂时就这样吧，毕竟还要考试

时间：2017.12.26

---

# 多线程发送文件修改说明

对server端和client端线程函数ThreadProcess的循环读写send和recv函数的退出机制做了修改，因为之前的退出会出现死循环的现象，视频文件还是不可以发送，只能看几分钟的，至今未找到原因，回头问老师，发送后的文件会比原文件大一点，iso、txt、doc、docx、rar文件均能成功发送

附一张效果图
![image_text](https://github.com/shangli-cuc/picture/blob/master/%E5%A4%9A%E7%BA%BF%E7%A8%8B%E5%8F%91%E9%80%81%E6%96%87%E4%BB%B6debug%E7%89%88%E6%9C%AC.png)

时间：2017.12.27

