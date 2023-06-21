//
// Created by fsindustry on 6/16/23.
//
//
// Created by fsindustry on 2023/4/10.
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

  while (1) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    // 5）循环等待客户端连接
    int clientfd = accept(listenfd, (struct sockaddr *) &client, &len);

    // 6）新开线程处理客户端请求
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, str_echo, &clientfd);
  }
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

  if (n < 0 && errno == EINTR) {
    goto again;
  } else if (n < 0) {
    close(clientfd);
    err_msg("str_echo: read error");
  }
  return nullptr;
}