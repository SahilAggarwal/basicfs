obj-m := basicfs.o

basicfs-objs := basicfs.o

all:
	make -C /lib/modules/build/$(shell uname -r)/build M=$(pwd) modules

clean:
	make -C /lib/modules/build/$(shell uname -r)/build M=$(pwd) clean
