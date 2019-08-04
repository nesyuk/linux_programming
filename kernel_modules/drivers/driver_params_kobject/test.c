#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

int main(){
    int BUF_SIZE = 1024;

    int fd = open("/sys/kernel/kobj_folder/my_kobj", O_RDONLY);
    if (fd == -1)
       perror("open");
    
    char buf[BUF_SIZE];
    read(fd, buf, BUF_SIZE);
    printf("%s", buf);
    assert(strcmp(buf, "hello world! \n\0") == 0);
    close(fd);
}
