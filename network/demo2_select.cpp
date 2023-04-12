//
// Created by fsindustry on 2023/4/10.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include<unistd.h>
#include <cstdio>


#define BUFFER_LENGTH  128


int main() {

  // 创建socket句柄，指定协议族，TCP流式协议
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == listenfd) return -1;

  // 创建socket监听地址
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET; //指定为TCP/IP协议族
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定监听地址
  server_addr.sin_port = htons(9999);

  // 绑定服务端地址
  if (-1 == bind(listenfd, (sockaddr *) &server_addr, sizeof(server_addr))) {
    return -2;
  }

  // 启动监听
  listen(listenfd, 10);

  fd_set rfds_in, wfds_in, rfds_out, wfds_out;
  FD_ZERO(&rfds_in); // 清空读事件监听句柄集合（accept也属于读事件）
  FD_ZERO(&wfds_in); // 清空写事件监听句柄集合

  FD_SET(listenfd, &rfds_in); // 注册服务端句柄到rfds，即监听服务端的读事件

  int lastfd = listenfd;
  unsigned char buffer[BUFFER_LENGTH] = {0}; // 0
  int ret = 0;

  // 进入监听循环
  while (1) {
    rfds_out = rfds_in;
    wfds_out = wfds_in;

    // 获取读写事件，分别放入rset和wset中
    int nready = select(lastfd + 1, &rfds_out, &wfds_out, NULL, NULL);

    // 处理服务端句柄的accept事件
    // 若包含来自listenfd读事件
    if (FD_ISSET(listenfd, &rfds_out)) {
      printf("listenfd --> \n");

      // 接收客户端监听句柄
      sockaddr_in client;
      socklen_t len = sizeof(client);
      int clientfd = accept(listenfd, (sockaddr *) &client, &len);

      // 注册读事件
      FD_SET(clientfd, &rfds_in);
      if (clientfd > lastfd) lastfd = clientfd;
    }

    // 遍历所有客户端句柄，处理读写事件
    for (int cur_fd = listenfd + 1; cur_fd <= lastfd; cur_fd++) {
      // 处理客户端读事件
      if (FD_ISSET(cur_fd, &rfds_out)) {

        // 读取客户端消息
        ret = recv(cur_fd, buffer, BUFFER_LENGTH, 0);
        if (0 == ret) {
          close(cur_fd);
          FD_CLR(cur_fd, &rfds_in);
        } else if (ret > 0) {
          printf("buffer: %s, ret: %d\n", buffer, ret);
          FD_SET(cur_fd, &wfds_in); // 注册写事件
        }

      }
      // 处理客户端写
      else if (FD_ISSET(cur_fd, &wfds_out)) {
        // 回复客户端消息
        ret = send(cur_fd, buffer, ret, 0);

        // 注销写事件，注册读事件
        FD_CLR(cur_fd, &wfds_in);
        FD_SET(cur_fd, &rfds_in);
      }
    }
  }
}