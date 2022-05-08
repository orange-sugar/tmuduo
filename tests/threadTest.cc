#include "tmuduo/base/ThreadPool.h"

#include <stdio.h>

using namespace tmuduo;

void func()
{
  printf("haha\n");
}

int main()
{
  ThreadPool pool(2);
  pool.pushTask(func);
}