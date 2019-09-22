obj-m += driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


load:
	sudo insmod driver.ko delays=150,500,850,50,1000,350,200

unload:
	sudo rmmod driver.ko

log:
	sudo dmesg -c
