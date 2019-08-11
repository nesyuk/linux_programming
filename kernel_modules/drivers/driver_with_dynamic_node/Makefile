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

test:
	MAJORNUMBER="$(shell sudo cat /dev/mycdev)";ls -la /dev/mycdev |grep --quiet $$MAJORNUMBER;if [ $$? -eq 0 ];then echo "success";else echo "failed";fi
