#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#define MYBUF_SIZE 4096

int main(){

   char buf[MYBUF_SIZE];
   ssize_t nbytes;

   int fd = open("/dev/mynode", O_RDWR);
  
   if (fd == -1){
       perror("open");
       return;
   }
   nbytes = write(fd, "hello world", strlen("hello world"));

   lseek(fd, 0, SEEK_SET);
   nbytes = write(fd, "y", strlen("y"));

   lseek(fd, 4, SEEK_CUR);
   nbytes = write(fd, "w", strlen("w"));

   lseek(fd, -5, SEEK_END);
   nbytes = write(fd, " ", strlen(" "));

   lseek(fd, 7, SEEK_SET);
   nbytes = write(fd, "mold\0", strlen("mold\0"));

   lseek(fd, 0, SEEK_SET);
   nbytes = read(fd, buf, MYBUF_SIZE);
   assert(strcmp(buf, "yellow mold\n\0") == 0);

   lseek(fd, 100, SEEK_SET);
   nbytes = read(fd, buf, MYBUF_SIZE);
   assert(strcmp(buf, "\n\0") == 0);

   lseek(fd, -100, SEEK_END);
   nbytes = read(fd, buf, MYBUF_SIZE);
   printf("%s\n", buf);
   assert(strcmp(buf, "yellow mold\n\0") == 0);

   printf("Success\n");
   close(fd);
}
