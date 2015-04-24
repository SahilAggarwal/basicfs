#include <linux/init.h>
#include <linux/module.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/version.h>

#include "basic.h"

struct dentry *basicfs_lookup(struct inode *,struct dentry *,unsigned int);

struct basicfs_inode * basicfs_get_inode(struct super_block *, uint64_t);

static struct inode_operations basicfs_inode_ops = {
	.lookup = basicfs_lookup
};


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static int basicfs_readdir(struct file *filp, struct dir_context *ctx)
#else
static int basicfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
#endif
{
	loff_t 				pos		;
	struct inode 			*inode		;
	struct super_block 		*sb		;
	struct buffer_head 		*bh		;
	struct basicfs_inode 		*bfs_inode	;
	struct basicfs_dir_record	*bfs_dir	;
	int 				i		;
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	pos 	= ctx->pos;
	inode 	= filp->f_path.dentry->d_inode;
#else
	pos 	= filp->f_pos;
	inode	= filp->f_dentry->d_inode;
#endif

	bfs_inode = inode->i_private;
	sb = inode->i_sb;

	printk(KERN_INFO "Inside Dir, inode:%ld\n",inode->i_ino);

	if(unlikely(!S_ISDIR(bfs_inode->mode))) {
		printk(KERN_ERR "[%llu] [%lu] inode not a Dir\n",
							bfs_inode->inode_no,
							inode->i_ino);
		return -ENOTDIR;
	}

	bh 	= sb_bread(sb, bfs_inode->data_block_nr);
	BUG_ON(!bh);
	bfs_dir = (struct basicfs_dir_record *)bh->b_data;

	if(pos) {
		return 0;
	}

	for(i=0; i < bfs_inode->children_count; i++) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
		dir_emit(ctx, bfs_dir->filename, BASICFS_MAX_FILENAME_LEN, 
				bfs_dir->inode_no, DT_UNKNOWN );
		ctx->pos += sizeof(struct basicfs_dir_record);	
#else
		filldir(dirent, record->filename, BASICFS_MAX_FILENAME_LEN,
				bfs_dir->inode_no, DT_UNKNOWN );
		filp->pos += sizeof(struct basicfs_dir_record);
#endif
		pos += sizeof(struct basicfs_dir_record);
		bfs_dir++;
	}
	brelse(bh);
	return 0;
}

const struct file_operations basicfs_dir_ops = {
	.owner = THIS_MODULE,
	.iterate = basicfs_readdir,
};
	


struct dentry *basicfs_lookup(struct inode *parent_inode,
				struct dentry *child_dentry, unsigned int flags)
{
	struct basicfs_inode       *parent  = parent_inode->i_private	;
	struct super_block         *sb 	    = parent_inode->i_sb	;
	struct buffer_head         *bh					;
	struct basicfs_dir_record  *bfs_dir;
	int i;
	
	bh = sb_bread(sb, parent->data_block_nr);
	bfs_dir = (struct basicfs_dir_record *)bh->b_data;
	
	for(i=0; i< parent->children_count; i++) {
		if(!strcmp(bfs_dir->filename, child_dentry->d_name.name)) {
			struct inode 		*inode;
			struct basicfs_inode 	*bfs_inode;
			
			bfs_inode 	= basicfs_get_inode(sb,bfs_dir->inode_no) ;
			inode 		= new_inode(sb)				  ;
			inode->i_ino 	= bfs_dir->inode_no			  ;
			inode_init_owner(inode, parent_inode, bfs_inode->mode)	  ;
			inode->i_sb 	= sb					  ;
			inode->i_op 	= &basicfs_inode_ops			  ;
			inode->i_fop	= &basicfs_dir_ops			  ;

			inode->i_atime  = inode->i_mtime = inode->i_ctime =       \
					  CURRENT_TIME				  ;
			inode->i_private= bfs_inode				  ;
			
			d_add(child_dentry, inode);
			return NULL;
		}
	}
	return NULL;
}

struct basicfs_inode *
basicfs_get_inode(struct super_block *sb, uint64_t inode_no)
{
	struct basicfs_inode	   *bfs_inode	= NULL		;
	struct buffer_head 	   *bh				;
	
	bh 	  =  sb_bread(sb,BASICFS_INODESTORE_BLOCK_NR)	;
	bfs_inode =  (struct basicfs_inode *)bh->b_data		;
	
	return (bfs_inode + inode_no - 1);
}


int basicfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode;
	struct buffer_head *bh;	
	struct basicfs_super_block *sb_disk;

	bh = sb_bread(sb,BASICFS_SB_BLOCK_NR);
	
	sb_disk = (struct basicfs_super_block *)bh->b_data;

	if(unlikely(sb_disk->magic != BASICFS_MAGIC)) {
		printk(KERN_ERR "Magic Number mismatch\n");
		return -EPERM;
	}

	if(unlikely(sb_disk->block_size != BASICFS_DEFAULT_BLOCK_SIZE)) {
		printk(KERN_ERR "Non standard block size\n");
		return -EPERM;
	}
	
	root_inode  		= new_inode(sb)			;
	root_inode->i_ino	= BASICFS_ROOTDIR_INODE_NR	;
	inode_init_owner(root_inode,NULL,S_IFDIR);
	root_inode->i_op 	= &basicfs_inode_ops		;
	root_inode->i_atime 	= 				\
	root_inode->i_mtime 	= 				\
	root_inode->i_ctime 	= CURRENT_TIME			;
	root_inode->i_fop 	= &basicfs_dir_ops		;

	root_inode->i_private	= basicfs_get_inode(sb,		\
				  BASICFS_ROOTDIR_INODE_NR)	;
	
	sb->s_root  		= d_make_root(root_inode)	;
	sb->s_fs_info		= sb_disk			;
	sb->s_magic		= BASICFS_MAGIC			;
	
	if(!sb->s_root)
		return -ENOMEM;
	
	return 0;
}

static struct dentry *
basicfs_mount(struct file_system_type *fs_type, int flags,
	      const char *dev_name, void *data)
{
	struct dentry *ret;
	ret = mount_bdev(fs_type, flags, dev_name, data, basicfs_fill_super);
	
	if(unlikely(IS_ERR(ret)))
		printk(KERN_ERR "Basicfs: Error mounting basicfs");
	else
		printk(KERN_INFO "Basicfs: Basicfs mounter on [%s]\n",dev_name);

	return ret;
}

static void basicfs_kill_sb(struct super_block *sb)
{
	printk(KERN_INFO "Baiscfs: Unmmount successful\n");
	return;
}

struct file_system_type basicfs_type = {
	.owner	 = THIS_MODULE,
	.name    = "basicfs",
	.mount   = basicfs_mount,
	.kill_sb = basicfs_kill_sb
};

static int basicfs_init(void)
{
	int ret;

	ret = register_filesystem(&basicfs_type);
	if(likely(ret == 0)) 
		printk(KERN_INFO "Basicfs: Registered");
	else
		printk(KERN_INFO "Basicfs: Failed to register\n");

	return 0;
}

static void basicfs_exit(void)
{
	int ret = 0;
	
	ret = unregister_filesystem(&basicfs_type);
        if(likely(ret == 0))
                printk(KERN_INFO "Basicfs: Unregistered\n");
        else
                printk(KERN_INFO "Basicfs: Failed to unregister\n");	
}

module_init(basicfs_init);
module_exit(basicfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sahil Aggarwal <sahilagg0693@gmail.com>");
