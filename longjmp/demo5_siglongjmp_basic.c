//
// Created by fsindustry on 2023/12/4.
//
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

jmp_buf env;

void handler(int signo) {
  printf("Caught signal %d\n", signo);
  siglongjmp(env, 1);
}

int main() {
  // Set up signal handler
  signal(SIGINT, handler);

  if (sigsetjmp(env, 1) == 0) {
    printf("Before longjmp\n");
    // Simulate a non-local jump in response to a signal
    raise(SIGINT);
  } else {
    printf("After longjmp\n");
  }

  return 0;
}