#include "tmuduo/base/ThreadPool.h"

#include <stdio.h>

void func()
{
  printf("haha\n");
}

int main()
{
  ThreadPool pool(2);
  pool.pushTask(func);
}