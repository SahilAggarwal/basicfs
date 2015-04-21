obj-m := basicfs.o

basicfs-objs := basic.o

KERNDIR ?= /lib/modules/$(shell uname -r)/build 

all:
	make -C $(KERNDIR) M=$(PWD) modules

clean:
	make -C $(KERNDIR) M=$(PWD) clean
