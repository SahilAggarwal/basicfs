#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/fs.h>
#include <string.h>

#include "basic.h"

int padding(int fd, char *buff, int size)
{
	int ret = write(fd,buff,size);
	if(ret != size) {
		printf("Failed to pad");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[] )
{
	int fd,nbytes;
	ssize_t ret;
	struct basicfs_super_block sb		;
	struct basicfs_inode 	   root_inode	;
	struct basicfs_inode	   welcomefile_inode	;

	char welcome_filename[] = "welcome";
	char welcome_body[]	= "Welcome to the Basicfs !!";

	const uint64_t WELCOMEFILE_INODE_NR 		= 2;
	const uint64_t WELCOMEFILE_DATABLOCK_NR 	= 3;

	char *block_padding;
	struct basicfs_dir_record bfs_dir;
	
	if(argc != 2) {
		printf("Usage: mkfs-basicfs <device>\n");
		return -1;
	}
	
	fd = open(argv[1],O_RDWR);

	if(fd == -1) {
		perror("Error opening device\n");
		return -1;
	}

	sb.version 	= 1				;
	sb.magic 	= BASICFS_MAGIC			;
	sb.block_size 	= BASICFS_DEFAULT_BLOCK_SIZE	;
	sb.inodes_count	= 2				;
	sb.free_blocks 	= ~0				;
	
	ret = write(fd,(char *)&sb,sizeof(sb));
		
	if(ret != BASICFS_DEFAULT_BLOCK_SIZE) {
		printf("Bytes written [%d] not equal to block size\n",(int)ret);
		ret = -1;
		goto exit;
	}

	printf("Supe block written\n");
	
	root_inode.mode 		= S_IFDIR			;
	root_inode.inode_no 		= BASICFS_ROOTDIR_INODE_NR	;
	root_inode.data_block_nr	= BASICFS_ROOTDIR_DATABLK_NR	;
	root_inode.children_count 	= 1				;

	ret = write(fd,(char *)&root_inode,sizeof(root_inode));

	if(ret != sizeof(root_inode)) {
		printf("Root inode not written\n");
		ret = -1;
		goto exit;
	}
	
	printf("Root inode writen\n");

	welcomefile_inode.mode 		= S_IFREG;
	welcomefile_inode.inode_no 	= WELCOMEFILE_INODE_NR;
	welcomefile_inode.data_block_nr	= WELCOMEFILE_DATABLOCK_NR;
	welcomefile_inode.file_size	= sizeof(welcome_body);
	
	ret = write(fd,(char *)&welcomefile_inode, sizeof(welcomefile_inode));
	
	if(ret != sizeof(welcomefile_inode)) {
		printf("Failed to write welcome file\n");
		ret = -1;
		goto exit;
	} 
	printf("Welcome file written\n");

	nbytes = BASICFS_DEFAULT_BLOCK_SIZE - sizeof(root_inode) - sizeof(welcomefile_inode);
	block_padding = malloc(nbytes);

	if(!padding(fd,block_padding,nbytes)) {
		printf("Padded\n");
	}
	else {
		ret = -1;
		goto exit;
	}

	strcpy(bfs_dir.filename,welcome_filename);
	bfs_dir.inode_no = WELCOMEFILE_INODE_NR;
	nbytes = sizeof(bfs_dir);
	
	ret = write(fd,(char *)&bfs_dir, sizeof(bfs_dir));
	
	if(ret != sizeof(bfs_dir)) {
		printf("Faile to write directory data\n");
		ret = -1;
		goto exit;
	}

	printf("Written directory stats");

	nbytes = BASICFS_DEFAULT_BLOCK_SIZE - sizeof(bfs_dir);
	block_padding = malloc(nbytes);

	if(!padding(fd,block_padding,nbytes)) {
		printf("Padded\n");
	} 
	else {
		ret = -1;
		goto exit;
	}

	nbytes 	= sizeof(welcome_body);
	ret   	= write(fd, welcome_body, nbytes);
	if(ret != nbytes) {
		printf("Writing file failed\n");
		ret = -1;
		goto exit;
	}

	printf("File written\n");

	exit:
		close(fd);
		return ret;
}
