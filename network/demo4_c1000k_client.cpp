//
// Created by fsindustry on 4/20/23.
//

#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFFER    128
#define MAX_EPOLLSIZE  (384*1024)
#define MAX_PORT    100

// 计算耗时间隔，入参类型为timeval
#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

int isContinue = 0;

int main(int argc, char **argv) {

  // 接收2个入参：ip port
  if (argc <= 2) {
    printf("Usage: %s ip port\n", argv[0]);
    exit(0);
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);
  int connections = 0;
  // 所有客户端写的内容都一样，故可以共享
  char wbuf[128] = {0};
  int port_index = 0;

  // 创建epoll句柄
  epoll_event events[MAX_EPOLLSIZE];
  int epoll_fd = epoll_create(MAX_EPOLLSIZE);
  // 拷贝模拟数据到缓冲区
  strcpy(wbuf, " Data From MulClient\n");

  // 创建sockaddr对象
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);

  // 开始统计时间
  timeval tv_begin;
  gettimeofday(&tv_begin, NULL);

  // 进入事件处理循环
  while (1) {
    if (++port_index >= MAX_PORT) port_index = 0;

    epoll_event ev;
    int sockfd = 0;

    // 最大创建340000个客户端连接
    if (connections < 340000 && !isContinue) {
      // 1. 创建socket句柄
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd == -1) {
        perror("socket");
        goto err;
      }

      // 2. 连接服务端
      addr.sin_port = htons(port + port_index);
      if (connect(sockfd, (sockaddr *) &addr, sizeof(sockaddr_in)) < 0) {
        perror("connect");
        goto err;
      }

      // 3. 设置socket为非阻塞状态
      int flags = fcntl(sockfd, F_GETFL, 0);
      flags |= O_NONBLOCK; // 将 O_NONBLOCK 标志位设置为 1
      fcntl(sockfd, F_SETFL, flags); // 将修改后的标志位写回到文件描述符中

      // 4. 设置允许复用socket
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) 1, sizeof((char *) 1));

      // 5. 向服务端发送消息
      sprintf(wbuf, "Hello Server: client --> %d\n", connections);
      send(sockfd, wbuf, strlen(wbuf), 0);

      // 6. 注册客户端socket到epoll
      ev.data.fd = sockfd;
      ev.events = EPOLLIN | EPOLLOUT;
      epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);

      connections++;
    }

    // 每创建1000个连接，处理一次epoll事件
    if (connections % 1000 == 999 || connections >= 340000) {
      timeval tv_cur;

      gettimeofday(&tv_cur, NULL);
      int time_used = TIME_SUB_MS(tv_begin, tv_cur);
      printf("connections: %d, sockfd:%d, time_used:%d\n", connections, sockfd, time_used);

      // 等待事件响应
      int nfds = epoll_wait(epoll_fd, events, connections, 100);

      // 遍历并处理epoll事件
      for (int i = 0; i < nfds; i++) {
        int clientfd = events[i].data.fd;

        // 处理写事件
        if (events[i].events & EPOLLOUT) {
          sprintf(wbuf, "data from %d\n", clientfd);
          send(sockfd, wbuf, strlen(wbuf), 0);
        }
          // 处理读事件
        else if (events[i].events & EPOLLIN) {
          // 不同客户端读到的不一样，故不能共享
          char rbuf[MAX_BUFFER] = {0};
          ssize_t length = recv(sockfd, rbuf, strlen(wbuf), 0);
          if (length > 0) { // 收到退出消息
            printf(" RecvBuffer:%s\n", rbuf);
            if (!strcmp(rbuf, "quit")) {
              isContinue = 0;
            }
          } else if (length == 0) { // 接收不到数据，则关闭fd
            printf(" Disconnect clientfd:%d\n", clientfd);
            connections--;
            close(clientfd);
          } else { // 异常情况
            if (errno == EINTR) continue;
            printf(" Error clientfd:%d, errno:%d\n", clientfd, errno);
            close(clientfd);
          }
        } else { // 未知类型异常
          printf(" clientfd:%d, errno:%d\n", clientfd, errno);
          close(clientfd);
        }
      }
    }

    usleep(500);
  }

  return 0;

err:
  printf("error : %s\n", strerror(errno));
  return 0;
}