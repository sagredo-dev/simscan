#include "readwrite.h"
#include "buffer.h"

char buffer_1_space[BUFFER_OUTSIZE];
static buffer it = {
  buffer_1_space, 0, sizeof buffer_1_space, 1,
  { .write_op = write }
};
buffer *buffer_1 = &it;
