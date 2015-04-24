obj-m := basicfs.o

basicfs-objs := basic.o

KERNDIR ?= /lib/modules/$(shell uname -r)/build 

all: ko mkfs-basicfs

ko:
	make -C $(KERNDIR) M=$(PWD) modules

mkfs-basicfs_SOURCES:
	mkfs-basicfs.c basic.h

clean:
	make -C $(KERNDIR) M=$(PWD) clean
	rm mkfs-basicfs
