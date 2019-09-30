#include <stdio.h>
#include <stdlib.h>
#include "cooper.h"

int main(int argc, char** argv) {
  char* str;

  if (argc == 3) {
    str = cooperToStr(argv[1], argv[2]);
    printf("%s", str);
  }
  else
    printf("Numero errato di argomenti!\n");
  
  free(str);
  
  return 0;
}
