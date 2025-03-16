#include <stdio.h>

int main(int argc, char *argv[])
{
  unsigned char a; // 8 bits; 1 byte

  // definir o valor inicial de c
  a = 255;  // 0x11111111 = (0xff)
  printf("a: %d (%x)\n", a, a);
  a+=1; // ????
  printf("a: %d (%x)\n", a, a);

  char b;
  b = 127; // 0x01111111
  printf("b: %d (%x)\n", b, b);

  b+=1; // ????
  printf("b: %d (%x)\n", b, b);

  return 0;
}