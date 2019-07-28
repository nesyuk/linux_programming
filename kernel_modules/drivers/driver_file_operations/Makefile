obj-m += driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod driver.ko

unload:
	sudo rmmod driver.ko

log:
	sudo dmesg -c

do_test:	test_clean
	gcc test.c -o test

test_clean:
	-rm test

