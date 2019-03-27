#include <stdio.h>
#include <stdlib.h>
#include "cooper.h"

int main(int argc, char** argv) {
  char** array;
  int len;
  
  if (argc == 3) {
    array = cooperToArray(argv[1], argv[2], &len);
    
    for (int i=0; i<len; i++) {
      printf("%s\n", array[i]);
      free(array[i]);
    }
    
    free(array);
  }
  else
    printf("Numero errato di argomenti!");
    
  return 0;
}
