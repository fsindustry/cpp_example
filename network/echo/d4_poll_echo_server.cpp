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

  int i, maxfd, connfd, sockfd;
  int nready;
  ssize_t n;
  char buf[BUFFER_LENGTH];
  int maxidx = 0;
  struct pollfd client[MAX_POLL_SIZE];
  for (int i = 0; i < MAX_POLL_SIZE; i++)
    client[i].fd = -1;
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
  client[0].fd = listenfd;
  client[0].events = POLLRDNORM;

  maxfd = listenfd;
  for (;;) {

    // 6）获取监听事件
    nready = poll(client, maxidx + 1, INFTIM);

    // 7）若有新的连接到来，则添加到监听的fd_set中
    if (client[0].revents & POLLRDNORM) {
      socklen_t len = sizeof(clientaddr);
      // 接收客户端连接
      connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &len);
      if (connfd < 0) { // 处理中断信号
        if (errno == EINTR)
          continue;
        else
          err_sys("accept error");
      }

      // 在client中找一个位置，存放connfd
      for (i = 1; i < MAX_POLL_SIZE; i++) {
        if (client[i].fd < 0) {
          client[i].fd = connfd;
          break;
        }
      }

      if (i == MAX_POLL_SIZE) {
        err_quit("too many clients");
      }

      // connfd注册读事件
      client[i].events = POLLRDNORM;
      if (connfd > maxfd) maxfd = connfd;
      if (i > maxidx) maxidx = i;
      if (--nready <= 0) continue;
    }

    // 8）若有客户端连接的读事件，则处理读事件
    for (i = 1; i <= maxidx; i++) {
      if ((sockfd = client[i].fd) < 0) continue;

      if (client[i].revents & POLLRDNORM) {
        if ((n = read(sockfd, buf, BUFFER_LENGTH)) <= 0) { // 连接关闭或出错，则关闭，并注销监听
          close(sockfd);
          client[i].fd = -1;
        } else { // 读取到数据，则回写数据给客户端
          write(sockfd, buf, n);
        }

        if (--nready <= 0) break;
      }
    }
  }

  return 0;
}