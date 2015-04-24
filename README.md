basicfs - Very simple fs for learning purpose. I am also beginner to this so idea is to learn using 
https://github.com/psankar/simplefs and then create ext2 inspired very simple filesystem. So created branch
to practice and learn. Master will have ext2 based filesystem later.

Design
------------------------------------------------------

 	 ----------------------------------------------------
       |       |       |          |              |            |
       | Super | Inode | Root Dir |    Data      |  ......... | 
       |       |       |          |              |            |
       | Block | Store |  Data    |    Blocks of |  ......... | 
       |       |       |          |              |            |
       |       |       |          |    Files     |            |  
	 ----------------------------------------------------

Steps to Test
------------------------------------------------------

$ make

$ sudo insmod basicfs.ko

$ dd bs=1M count=100 if=/dev/zero of=image

$ mount -o loop -t basicfs image /mnt

$ dmesg | tail -10   [Check log messages

Usable commands
------------------------------------------------------

$ cd

$ ls /mnt 

