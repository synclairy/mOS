# 北航2020届计算机学院操作系统实验代码
BUAA-OS-2022

### 本仓库代码在以下测试中均获得满分
- lab0-lab6课下测试
- lab1-lab5期末强测
- lab0-lab5所有exam
- lab0-2-Extra	[考察基础]
- lab2-2-Extra	[实现快速分配Page和页迁移]
- lab3-1-Extra	[实现信号量PV操作]
- lab3-2-Extra	[实现访存异常的handler,因未考虑延迟槽，扣了1分，属于是助教手下留情]
- lab4-1-Extra	[对IPC机制进行修改使得轮询的进程可以被阻塞]
- lab5-2-Extra	[接受以path为绝对路径的文件夹，将此文件夹的所有文件的文件名以空格(space)为间隔保存在字符串ans中]


### lab4挑战性任务	(lab4-challenge分支)
按照 POSIX 标准，结合 MOS 操作系统已有的架构实现线程机制和信号量机制。

#### POSIX Threads
POSIX 线程(POSIX Threads)是由 IEEE Std 1003.1c 定义的一套线程标准。

你需要至少实现标准中的以下函数(省略参数):
```
pthread_create();
pthread_exit();
pthread_cancel();
pthread_join();
```

#### POSIX Semaphore

POSIX Semaphore 是由 IEEE Std 1003.1b 标准定义的一套信号量标准。该标准提供的信号量分为有名和无名信号量，无名信号量用于进程内同步与通信，有名信号量可以用于进程内同步与通信，也可以用于进程间同步与通信。

你需要至少实现标准中的以下函数(省略参数):
```
sem_init();
sem_destroy();
sem_wait();
sem_trywait();
sem_post();
sem_getvalue();
```