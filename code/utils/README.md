## Buffer

- 使用 STL 中的 vector 实现缓冲区的自动增长

- 维护缓冲区的开始/终止下标，开始/终止地址，读写位置，可写可读长度等。

- 当空间不足时用 resize 重新声明缓冲区大小，实现空间扩展。

- 对于string，buffer，char*等使用指针类型转化写入缓冲区。

- 往文件写直接一次写完，读取文件为了防止缓冲区空间不够采用readv+iovec实现分散读取。


## lock

- 封装实现信号量，互斥锁，条件变量


## Timer

- 定时器容器采用 vector 模拟最小堆来实现对超时线程的处理，可以自动按照时间顺序维护堆内定时器。

- 定时器编号与位置关系通过 unordered_map 维护。

- 使用 chrono::high_resolution_clock 维护时间，使用 chrono::duration_cast 实现时间戳转化。


## epoller

- 封装epoll类