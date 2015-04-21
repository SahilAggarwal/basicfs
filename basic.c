#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#include "basicfs.h"

struct inode *
basicfs_get_inode(struct super_block *sb,const struct inode *dir,
		  umode_t mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);

	if(inode) {
		
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		
		switch(mode & S_IFMT) {
		
		case S_IFDIR:
			inc_nlink(inode);
			break;

		case S_IFREG:
		case S_IFLNK:
		default:
			printk(KERN_ERR 
			       "basicfs: cant create meaningful inode only for root\n");
			return NULL;
			break;
		}		
	}
	return inode;
}

int basicfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct buffer_head *bh;
	struct basicfs_sb_info *sbi;
	struct basicfs_super_block *es;
	struct inode *root;
	long ret = -EINVAL;
	int blocksize = BASICFS_DEFAULT_BLOCK_SIZE;

	int err;
	err = -ENOMEM;
	sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
	if(!sbi)
		goto failed;

	sbi->s_blockgroup_lock = 
			kzalloc(sizeof(struct blockgroup_lock),GFP_KERNEL);
	if(!sbi->s_block_group_lock) {
		kfree(sbi);
		goto failed;
	}

	sb->sb_fs_info = sbi;
	sbi->s_sb_block = BASICFS_SB_BLOCK_NR;

	bh = sb_bread(sb, BASICFS_SB_BLOCK_NR);
	if(!bh) {
		printk(KERN_ERR "Failed to read superblock\n");
		goto failed_sbi;	
	}

	es = (struct basicfs_super_block)((char *)bh->b_data);
	sbi->s_es = es;
	
	if(unlikely(es->magic != BASICFS_MAGICNO)) {
		printk(KERN_ERR "Magic no doesnt match\n");
		goto failed_sbi;
	}
	
	inode       = basicfs_get_inode(sb, NULL, S_IFDIR, 0);
	sb->s_root  = d_make_root(inode);
	
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
