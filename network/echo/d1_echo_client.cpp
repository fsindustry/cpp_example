//
// Created by fsindustry on 6/16/23.
//


#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>

extern "C" {
#include "unp.h"
}

#define BACKLOG_SIZE 10
#define BUFFER_LENGTH  1024

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {

  // 0) 接收參數
  if (argc != 3){
    err_quit("usage: program <IPaddress> <Port>");
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);

  // 1）创建socket句柄
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (sockfd == -1) return -1;

  // 2）封装socket地址
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  // 3）连接服务器
  if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(sockaddr_in)) < 0) {
    err_quit("connect server %s:%d failed.", ip, port);
  }

  // 4）接收控制台输入，发送给服务端
  str_cli(stdin, sockfd);

  return 0;
}

void str_cli(FILE *fp, int sockfd){
  char write_buf[BUFFER_LENGTH], read_buf[BUFFER_LENGTH];
  while(Fgets(write_buf, BUFFER_LENGTH, fp)){
    Writen(sockfd, write_buf, strlen(write_buf));
    if(Readline(sockfd, read_buf,BUFFER_LENGTH) == 0){
      err_quit("str_cli: server terminated prematurely");
    }

    Fputs(read_buf,stdout);
  }
}