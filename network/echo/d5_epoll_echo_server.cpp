//
// Created by fsindustry on 6/16/23.
//

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <cerrno>
#include <sys/epoll.h>

extern "C" {
#include "unp.h"
}

#define SERVER_PORT 9999
#define BACKLOG_SIZE 10
#define BUFFER_LENGTH  1024
#define MAX_POLL_SIZE 4096

void *str_echo(void *args);

void sig_chld(int signo);

int main() {

  int connfd, sockfd;
  int nready;
  ssize_t n;
  char buf[BUFFER_LENGTH];
  int epollfd = epoll_create(1);
  epoll_event ev, events[MAX_POLL_SIZE];

  struct sockaddr_in clientaddr;


  // 1）创建socket句柄
  int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (listenfd == -1) return -1;

  // 2）封装socket地址
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);

  // 3）bind端口
  if (-1 == bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
    return -2;
  }

  // 4）开启监听
  listen(listenfd, BACKLOG_SIZE);

  // 5）注册listenfd读事件
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

  for (;;) {

    // 6）获取监听事件
    nready = epoll_wait(epollfd, events, MAX_POLL_SIZE, -1);

    for (int i = 0; i < nready; i++) {
      int eventfd = events[i].data.fd;

      // 7）若有新的连接到来，则添加到监听的fd_set中
      if (listenfd == eventfd) {

        socklen_t len = sizeof(clientaddr);
        // 接收客户端连接
        connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &len);
        if (connfd < 0) { // 处理中断信号
          if (errno == EINTR)
            continue;
          else
            err_sys("accept error");
        }

        // connfd注册读事件
        ev.events = EPOLLIN;
        ev.data.fd = connfd;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
      }
        // 8）若有客户端连接的读事件，则处理读事件
      else if (events[i].events & EPOLLIN) {

        if ((sockfd = events[i].data.fd) < 0) continue;

        if ((n = read(sockfd, buf, BUFFER_LENGTH)) <= 0) { // 连接关闭或出错，则关闭，并注销监听
          epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, nullptr);
          close(sockfd);
        } else { // 读取到数据，则回写数据给客户端
          write(sockfd, buf, n);
        }
      }
    }
  }

  return 0;
}