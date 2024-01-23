obj-m := main.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KER_DIR=/home/minkha/workspace/learn-ldd-udemy/sources/linux-bbb-4.14/

all:
	sudo make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KER_DIR) M=$(PWD) modules
clean:
	sudo make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KER_DIR) M=$(PWD) clean
help:
	sudo make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KER_DIR) M=$(PWD) help