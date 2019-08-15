obj-m += driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:	create_dev
	sudo insmod driver.ko

unload:	remove_dev	
	sudo rmmod driver.ko

create_dev:
	sudo mknod /dev/mycdev c 240 0
	sudo chmod 777 /dev/mycdev

remove_dev:
	-sudo rm /dev/mycdev

log:
	sudo dmesg -c

test:	clean_test
	gcc test.c -o test.o
	./test.o

clean_test:
	-rm test.o
