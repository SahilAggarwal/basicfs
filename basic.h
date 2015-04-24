
#define BASICFS_MAGIC			0x10032013
#define BASICFS_DEFAULT_BLOCK_SIZE 	4096
#define BASICFS_MAX_FILENAME_LEN	255

#define BASICFS_SB_BLOCK_NR		0
#define BASICFS_INODESTORE_BLOCK_NR	1
#define BASICFS_ROOTDIR_DATABLK_NR	2

#define BASICFS_ROOTDIR_INODE_NR	1

struct basicfs_dir_record {
	char 		filename[BASICFS_MAX_FILENAME_LEN];
	uint64_t 	inode_no;
};

struct basicfs_inode {
	mode_t		mode;
	uint64_t	inode_no;
	uint64_t	data_block_nr;

	union {
		uint64_t file_size;
		uint64_t children_count;
	};
};

struct basicfs_super_block {
	uint64_t 		version;
	uint64_t 		magic;
	uint64_t 		block_size;
	uint64_t		inodes_count;
	uint64_t 		free_blocks;

	char padding[4096 - (5 * sizeof(uint64_t))];
};
