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

void *str_echo(void *args);

void sig_chld(int signo);

int main() {

  int i, maxfd, connfd, sockfd;
  int nready;
  ssize_t n;
  fd_set rset, allset;
  char buf[BUFFER_LENGTH];
  int maxidx = -1;
  int client[FD_SETSIZE];
  memset(client, -1, FD_SETSIZE);
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
  FD_ZERO(&allset);
  FD_SET(listenfd, &allset);

  maxfd = listenfd;
  for (;;) {

    // 6）获取监听事件
    // 每次从现有fd_set中拷贝一份，作为select()入参，防止状态被select更改；
    rset = allset;
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

    // 7）若有新的连接到来，则添加到监听的fd_set中
    if (FD_ISSET(listenfd, &rset)) {
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
      for (i = 0; i < FD_SETSIZE; i++) {
        if (client[i] < 0) {
          client[i] = connfd;
          break;
        }
      }

      if (i == FD_SETSIZE) {
        err_quit("too many clients");
      }

      // 注册connfd到监听fd_set中
      FD_SET(connfd, &allset);
      if (connfd > maxfd) maxfd = connfd;
      if (i > maxidx) maxidx = i;
      if (--nready <= 0) continue;
    }

    // 8）若有客户端连接的读事件，则处理读事件
    for (i = 0; i <= maxidx; i++) {
      if ((sockfd = client[i]) < 0) continue;

      if (FD_ISSET(sockfd, &rset)) {
        if ((n = read(sockfd, buf, BUFFER_LENGTH)) <= 0) { // 连接关闭或出错，则关闭，并注销监听
          close(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        } else { // 读取到数据，则回写数据给客户端
          write(sockfd, buf, n);
        }

        if (--nready <= 0) break;
      }
    }
  }

  return 0;
}