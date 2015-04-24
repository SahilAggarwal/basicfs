basicfs - Very simple fs for learning purpose 

Design
============================================================

 ----------------------------------------------------
|       |       |           |           |            |
| Super | Inode | Root Dir  | Data      | .......... |
| Block | Store | Data      | Blocks of | .......... |
|       |       |           | Files     |            |
 ----------------------------------------------------

Steps to Test
============================================================

$ make

$ sudo insmod basicfs.ko

$ dd bs=1M count=100 if=/dev/zero of=image

$ mount -o loop -t basicfs image /mnt

$ dmesg | tail -10   [Check log messages

Usable commands
=============================================================

$ cd

$ ls /mnt 

