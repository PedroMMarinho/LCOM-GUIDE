#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
  if(!(argv[1][0] == '0' || atoi(argv[1]))){
    printf("This is not a number\n");
    return 1;
  }
  int value;
  value = atoi(argv[1]);
  printf("Read Value: %d\n", value);
  return 0;
}