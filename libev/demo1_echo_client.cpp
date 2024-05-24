//
// Created by fsindustry on 5/24/24.
//

#include <iostream>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>
#include <ev.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 3033
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

ev_io stdin_watcher;
ev_io socket_watcher;

struct io_buf {
    char *data;
    size_t len;
};

int set_nonblocking(int fd) {
    int flags = 1;
    if (ioctl(fd, FIONBIO, &flags) == -1) {
        perror("ioctl FIONBIO");
        return -1;
    }
    return 0;
}

void stdin_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void conn_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main() {
    std::cout << "Hello World!" << std::endl;
    struct ev_loop *loop = EV_DEFAULT;
    int connfd;
    struct sockaddr_in addr;

    // 1. create socket
    connfd = socket(PF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        perror("socket error");
        return -1;
    }

    // 2. connect to server
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    if (connect(connfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        if (errno != EINPROGRESS) {
            perror("connect error");
            close(connfd);
            return -1;
        }
    }

    // 3. register stdin / stdout watcher
    struct io_buf *buf = (io_buf *) malloc(sizeof(struct io_buf));
    buf->data = (char *) malloc(BUFFER_SIZE);
    bzero(buf->data, BUFFER_SIZE);
    buf->len = 0;
    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    stdin_watcher.data = buf;
    ev_io_start(loop, &stdin_watcher);

    // 4. register connfd watcher
    ev_io_init(&socket_watcher, conn_cb, connfd, EV_READ);
    socket_watcher.data = buf;
    ev_io_start(loop, &socket_watcher);

    // 5. start eventloop
    ev_run(loop, 0);

    // 6. destory eventloop
    ev_loop_destroy(loop);
}

void stdin_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    int connfd = socket_watcher.fd;
    io_buf *buf = (io_buf *) watcher->data;
    if (EV_READ & revents) { // stdin read event
        ssize_t n = read(STDIN_FILENO, buf->data, BUFFER_SIZE);
        if (n < 0) {
            perror("read from stdin error.");
            return;
        }

        write(connfd, buf->data, n);
        bzero(buf->data, BUFFER_SIZE);
        buf->len = 0;
    }
}

void conn_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    int connfd = watcher->fd;
    io_buf *buf = (io_buf *) watcher->data;
    if (EV_READ & revents) { // read event
        ssize_t n = read(connfd, buf->data, BUFFER_SIZE);
        if (n < 0) {
            if (errno != EAGAIN) {
                perror("read from server error.");
                ev_io_stop(loop, watcher);
                close(connfd);
            }
            return;
        } else if (n == 0) {
            ev_io_stop(loop, watcher);
            close(connfd);
            perror("Connection closed by peer.");
            return;
        }

        write(STDOUT_FILENO, buf->data, n);
        bzero(buf->data, BUFFER_SIZE);
        buf->len = 0;
    }
}