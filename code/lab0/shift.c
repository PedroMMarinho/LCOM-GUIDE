#define SHIFT(val,n) (val << (n))
#include <stdio.h>
int main(int argc, char *argv[])
{
  char val = 8; // 0000 1000
  int n = 2; // number of shifts
  
  char newVal = SHIFT(val,n); // same as *2^n, 8 * 2^2 = 32

  printf("Value is %d\n", newVal);
  printf("Value in hexa 0x%x\n", newVal); // 0010 0000

  newVal = SHIFT(newVal,3);

  printf("Value is %d\n", newVal); // 0
  printf("Value in hexa 0x%x\n", newVal); // 0000 0000 Exceeded the number of bits
  return 0;
}