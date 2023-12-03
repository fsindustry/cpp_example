//
// Created by fsindustry on 2023/12/3.
//
#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

void foo() {

  printf("bar: Before longjmp\n");
  longjmp(env, 1);  // Simulate a non-local jump to setjmp
  printf("bar: After longjmp\n");

  printf("foo: Before setjmp\n");
  setjmp(env);
  printf("foo: After setjmp\n");
}

int main() {
  printf("main: Before foo\n");
  foo();
  printf("main: After foo\n");

  return 0;
}