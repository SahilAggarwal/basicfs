#define BASICFS_MAGIC 			0x10032013

#define BASICFS_DEFAULT_BLOCK_SIZE 	4096
#define BASICFS_FILENAME_MAXLEN   	255

const int BASICFS_SB_BLOCK_NR	= 0;


struct basicfs_super_block {
	__le32	s_inodes_count;
	__le32	s_blocks_count;
	__le32	s_r_blocks_count;
	__le32	s_free_blocks_count;
	__le32	s_free_inodes_count;
	__le32	s_first_data_block;
	__le32	s_log_block_size;
	__le32	s_blocks_per_group;
	__le32	s_inodes_per_group;
	__le32	s_mtime;
	__le32	s_wtime;
	__le32	s_magic;
	__le32	s_state;
	__le32	s_first_ino;
	__le16	s_inode_size;
	__le16	s_block_group_nr;
	
};

struct basicfs_sb_info {
	unsigned long s_inodes_per_block; /* No of inodes per block       */
	unsigned long s_blocks_per_group; /* No of blocks per group       */
	unsigned long s_inodes_per_group; /* No of inodes per group 	  */
	unsigned long s_itb_per_group;    /* No of inode tables per group */
	unsigned long s_gdb_count;	  /* No of group desc blocks	  */
	unsigned long s_desc_perf_block;  /* No of group desc per block   */
	unsigned long s_groups_count;	  /* No of groups in fs           */
	struct buffer_head *s_sbh;	  /* Buffer containing super block*/
	struct basicfs_super_block *s_es; /* Pointer to sb in buffer      */
	struct buffer_head **s_group_desc;/* Pointer to group desc in buff*/
	unsigned long s_mount_opt;
	unsigned long s_sb_block;
	kuid_t s_resuid;
	kuid_t s_resgid;
	int s_first_ino;
	struct blockgroup_lock *s_blockgroup_lock;
	
};

#endif
