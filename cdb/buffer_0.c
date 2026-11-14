#include "readwrite.h"
#include "buffer.h"

static ssize_t buffer_0_read(int fd, void *buf, size_t len)
{
  if (buffer_flush(buffer_1) == -1) return -1;
  return read(fd,buf,len);
}

char buffer_0_space[BUFFER_INSIZE];
static buffer it = {
  buffer_0_space, 0, sizeof buffer_0_space, 0,
  { .read_op = buffer_0_read }
};
buffer *buffer_0 = &it;
