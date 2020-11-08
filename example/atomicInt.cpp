#include <iostream>
#include <atomic>
#include <stdio.h>

using namespace std;

int main()
{
  atomic<int> i(3);

  ++i;

  i+=7;

  int x=i;

  atomic<int> *pi= new atomic<int>(33);

  cout << x << endl;

  printf("%d\n",(int)(*pi));
}
