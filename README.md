# WebServer

- 用 C++11 实现的高性能 Web 服务器，经过 webbenchh 压力测试可以实现上万的 QPS

- Linux + MySQL + C++11

## Technical points

- 封装Epoller（包括添加文件监听，修改文件监听，删除文件监听，定时监听）利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；

- 利用标准库容器 vector 封装 char数组，实现自动增长的缓冲区。写入前先判断缓冲区空间大小，不足够那么进行扩充或者清除已经写入文件的缓冲区，读取文件采用分散写 readv + iov 保证数据全部一次性读入。

- 基于 vector 手动实现小根堆，构造时间戳结点实现的定时器，超时调用回调函数并且关闭超时的非活动连接。

- 数据库连接池采用单例模式懒汉模式，利用RAII机制实现了数据库志愿的连接，减少数据库连接建立与关闭的开销，同时实现了用户注册登录功能。

- 线程池采用 queue 保存任务队列，子线程创建采用 Lambda 表达式实现后进行线程分离，不断从任务队列取出任务执行。
  
- 封装lock类实现信号量，条件变量和互斥锁。

- 利用正则与状态机解析HTTP请求报文，构造响应报文写出，实现处理静态资源的请求，将请求的文件映射到共享内存中提高文件访问速度。

- c++新特性：
  - cast<static> 实现数据类型强转
  - const_cast<T> 去除复合类型中const和volatile属性
  - atomic<T> 实现读写的原子操作 
  - 使用 lock_guard<std::mutex> 和 lock_unique<std::mutex> 实现数据共享区的同步访问。
  - 使用智能指针 shared_ptr<T> 和 unique_ptr<T> 对线程池等进行操作。
  - bind 函数适配器转换为适配线程池参数的函数类型。

- 使用模板和宏定义，利用单例模式与阻塞队列实现异步的日志系统，记录服务器运行状态；



## 项目结构
```
.
├── Code           源代码
│   ├── http
│   ├── log
│   ├── pool
│   │   ├── sqlconnpool   
│   │   ├── threadpool   
│   ├── server
│   ├── utils
│   │    ├── buffer
│   │    ├── epoller
│   │    ├── lock
│   │    ├── timer
│   └── main.cpp
│
├── Resources      静态资源
│   ├── index.html
│   ├── image
│   ├── video
│   ├── js
│   └── css
│
├── Bin            可执行文件
│   └── server
│
├── Log            日志文件
│
├── Webbench-1.5   压力测试
│
├── Build          make
│   └── Makefile
│
├── Makefile       Makefile
│
└── README.md      readme
```

## 项目启动

```
// 建立yourdb库
create database yourdb;

// 创建user表
USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

// 添加数据
INSERT INTO user(username, password) VALUES('name', 'password');
```

```
make
./bin/server
```

## 压力测试

```
./webbench-1.5/webbench -c 100 -t 10 http://ip:port/
./webbench-1.5/webbench -c 1000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 5000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 10000 -t 10 http://ip:port/
```

**QPS 10000+**