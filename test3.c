#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  char* var;
  char** vars = malloc(sizeof(char*));
  vars[0] = strtok(argv[2], " ");
  int varLen = 1;
  
  while ((var = strtok(NULL, " ")) != NULL) {
    varLen++;
    vars = realloc(vars, sizeof(char*) * varLen);
    vars[varLen - 1] = var;
  }

  test(argv[1], vars, varLen);
 
  
  return 0;
}
