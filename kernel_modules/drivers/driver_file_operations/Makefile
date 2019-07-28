obj-m += driver.o

all:	clean create_node
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:	remove_node
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod driver.ko

unload:
	sudo rmmod driver.ko

log:
	sudo dmesg -c

create_node:
	sudo mknod /dev/mynode c 700 0
	sudo chmod 777 /dev/mynode

remove_node:
	-sudo rm /dev/mynode

do_test:	test_clean
	gcc test.c -o test

test_clean:
	-rm test
