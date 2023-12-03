//
// Created by fsindustry on 2023/12/3.
//

#include <stdio.h>
#include <setjmp.h>
#include <pthread.h>

jmp_buf env;

void* thread_func(void* arg) {
  printf("Thread: Before longjmp\n");
  longjmp(env, 1);  // Jump back to setjmp, but in a different thread
  printf("Thread: After longjmp\n");  // This code is never executed
  return NULL;
}

int main() {
  pthread_t thread;

  if (setjmp(env) == 0) {
    printf("Main: Before thread creation\n");
    pthread_create(&thread, NULL, thread_func, NULL);
    pthread_join(thread, NULL);
    printf("Main: After thread join\n");  // This code is never executed
  } else {
    printf("Main: After longjmp\n");
  }

  return 0;
}
