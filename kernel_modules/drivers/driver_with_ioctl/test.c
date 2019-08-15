#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define IOC_MAGIC 't'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

int main(){

    int fd = open("/dev/mycdev", O_NONBLOCK, O_RDONLY);
    assert(fd != -1);

    unsigned int MYBUF_SIZE = 1024; 
    char buf[MYBUF_SIZE];
    
    read(fd, buf, MYBUF_SIZE);
    assert(strcmp(buf, "0 0\0") == 0);

    ioctl(fd, SUM_LENGTH, "hello ");
    ioctl(fd, SUM_LENGTH, "world");
    ioctl(fd, SUM_CONTENT, "100");
    ioctl(fd, 123, "!");
    read(fd, buf, MYBUF_SIZE);
    assert(strcmp(buf, "11 100\0") == 0);

    close(fd);
    printf("success!\n");
}
