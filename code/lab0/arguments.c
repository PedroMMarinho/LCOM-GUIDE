#include <stdio.h>
int main(int argc, char *argv[])
{
  int i = 0;
  for (i; i < argc; i++)
    printf("%d Parameter: %s\n", i, argv[i]);
  return  0;
}