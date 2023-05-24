//
// Created by fsindustry on 2023/5/24.
//

#include <stdio.h>
#include <stdlib.h>

void fun() {
  int *p = (int *) malloc(10 * sizeof(int));
  p[10] = 0;
}

int main(void) {
  fun();
  return 0;
}