#include "fasm_utils.h"

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void fasm_assemble(const char *output_file, const char *asm_file) {
  char command[512];
  // might be dangerous :3
  snprintf(command, sizeof(command), "fasm %s %s.o > /dev/null", asm_file,
           output_file);
  int result = system(command);
  if (result != 0) {
    scu_perror(NULL, "Assembly failed with code %d\n", result);
    exit(1);
  }
}
