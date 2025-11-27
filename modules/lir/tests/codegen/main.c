#include "hello_world.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <test>", argv[0]);
    return 1;
  }

  const char *test = argv[1];

  if (!strcmp(test, "lirCodegenTests/hello-world")) test_hello_world();
  else {
    fprintf(stderr, "unknown test %s", test);
    return 1;
  }

  return 0;
}
