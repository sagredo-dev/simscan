#include "buffer.h"

void buffer_init(buffer *s,buffer_op_write op,int fd,char *buf,unsigned int len)
{
  s->x = buf;
  s->fd = fd;
  s->op.write_op = op;
  s->p = 0;
  s->n = len;
}
