#include <unistd.h>
#include <stdio.h>

int main() {
  printf("Hello timer-test\n");
  int sec = 1;
  while(1)
  {
    while(gettimeofday() / 500000 < sec);
    printf("Hello timer-test %d\n",sec);
    sec += 1;
  }
  return 0;
}
