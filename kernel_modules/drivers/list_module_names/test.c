#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define MY_BUF_SIZE 1024

int main(){
    
     int fd = open("/sys/kernel/mydev/mydata", O_RDONLY);
     assert(fd != -1);

     char buf[MY_BUF_SIZE];
     read(fd, buf, MY_BUF_SIZE);
     
     printf("%s\n", buf);
     close(fd);
}
