#include "readwrite.h"
#include "buffer.h"

char buffer_1small_space[256];
static buffer it = {
  buffer_1small_space, 0, sizeof buffer_1small_space, 1,
  { .write_op = write }
};
buffer *buffer_1small = &it;
