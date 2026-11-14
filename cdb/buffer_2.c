#include "readwrite.h"
#include "buffer.h"

char buffer_2_space[256];
static buffer it = {
  buffer_2_space, 0, sizeof buffer_2_space, 2,
  { .write_op = write }
};
buffer *buffer_2 = &it;
