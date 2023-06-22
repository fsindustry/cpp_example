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

  // 5）注册SIGCHLD信号处理函数，处理子进程信号
  Signal(SIGCHLD, sig_chld);

  pid_t childpid;
  while (1) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    // 5）循环等待客户端连接
    int clientfd = accept(listenfd, (struct sockaddr *) &client, &len);
    if (clientfd < 0) { // 处理中断信号
      if (errno == EINTR)
        continue;
      else
        err_sys("accept error");
    }

    // 6）新开子进程处理客户端请求
    if ((childpid = fork()) == 0) { // child process
      close(listenfd);
      str_echo(clientfd);
      exit(0);
    } else { // parent process
      close(clientfd);
    }
  }

  return 0;
}

// 客户端处理线程
void *str_echo(void *args) {
  int clientfd = *(int *) args;
  unsigned char buffer[BUFFER_LENGTH] = {0};
  ssize_t n;
  again:
  // 读取客户端输入数据，并回写客户端
  while ((n = read(clientfd, buffer, BUFFER_LENGTH)) > 0) {
    write(clientfd, buffer, n);
    printf("buffer: %s, ret: %d\n", buffer, n);
  }

  if (n < 0 && errno == EINTR) { // 处理中断信号，直接重试
    goto again;
  } else if (n < 0) { // 其它错误关闭fd
    close(clientfd);
    err_msg("str_echo: read error");
  } else if (n == 0) { // 若对端关闭，则也关闭fd
    close(clientfd);
  }
  return nullptr;
}

// SIGCHLD 信号处理函数
void sig_chld(int signo) {
  pid_t pid;
  int stat;

  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return;
}