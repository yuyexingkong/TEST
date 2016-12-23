#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
  int fd;
  int ret;
  unsigned char *buf;
  struct stat stat;
  void *res;
  unsigned int ImageBase ;
  int rebase[2] = {0x0B, 0x12};
  unsigned char *temp;

  ImageBase = 0x50000000;

  fd = open("hello.bin", O_RDWR);
  if (fd < 0) {
    perror("fd");
    return -1;
  }

  ret = fstat(fd, &stat);
  if (ret < 0) {
    perror("fstat");
    return -1;
  }
  
  buf = (unsigned char *)malloc(stat.st_size);
  if (buf == NULL) {
    perror("allocate");
    return -1;
  }

  ret = read(fd, buf, stat.st_size);
  if (ret < 0 || ret != stat.st_size) {
    perror("read");
    return -1;
  }

  res = mmap((void *)ImageBase, stat.st_size, PROT_EXEC|PROT_WRITE|PROT_READ, MAP_PRIVATE, fd, 0);
  if (res == MAP_FAILED) {
    perror("mmap");
    return -1;
  }
  printf("%p \n", res);

  for (int i = 0; i < 2; i++)
  {
    *(unsigned int *)((char *)res + rebase[i]) += ImageBase;
  }
  
  temp = (unsigned char *)res;
  for (int i = 0; i < 0x100; i++)
    printf("%02X%s", *temp++, (i+1) % 0x10 ? " ": "\n");

  __asm__  volatile ("\n\t"\
             "movl $1 ,%%eax\n\t"\
             "jmp *%0\n\t"\
             ::"mem"(res):
  );


}

