obj-m += driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod driver.ko my_chars=hello,world!

unload:
	sudo rmmod driver

log:
	sudo dmesg -c

do_test:	clean_test
	gcc test.c -o test
	./test

clean_test:
	-rm test
