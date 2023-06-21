//
// Created by fsindustry on 2023/4/10.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_LENGTH  128

void *routine(void *args);

int main() {

  // 1）创建socket句柄
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) return -1;

  // 2）封装socket地址
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(9999);

  // 3）bind端口
  if (-1 == bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
    return -2;
  }

  // 4）开启监听
  listen(listenfd, 10);

  while (1) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    // 5）循环等待客户端连接
    int clientfd = accept(listenfd, (struct sockaddr *) &client, &len);

    // 6）新开线程处理客户端请求
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, routine, &clientfd);
  }
}

// 客户端处理线程
void *routine(void *args) {
  int clientfd = *(int *) args;

  while (1) {
    unsigned char buffer[BUFFER_LENGTH] = {0};
    // 7）读取客户端输入数据
    int ret = recv(clientfd, buffer, BUFFER_LENGTH, 0);
    if (ret == 0) {
      close(clientfd);
      break;
    }

    printf("buffer: %s, ret: %d\n", buffer, ret);
    // 8）向客户端发送数据
    ret = send(clientfd, buffer, ret, 0);
  }
  return nullptr;
}