#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

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
			       "basicfs can create meaningful inode only for root\n")
			return NULL;
			break;
		}		
	}
}


int basicfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	
	sb->s_magic = 0x10032013;
	
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
	
	if(unlikely(IS_ERR(ret))
		printk(KERN_ERR "Basicfs: Error mounting basicfs");
	else
		printk(KERN_INFO "Basicfs: Basicfs mounter on [%s]\n",dev_name);

	return ret;
}


struct file_system_type basicfs_type {
	.owner	 = THIS_MODULE;
	.name    = "basicfs";
	.mount   = basicfs_mount;
	.kill_sb = basicfs_kill_sb;
};

static int basicfs_init(void)
{
	printk(KERN_INFO "Basicfs: initializing...\n");

	int ret;
	
	ret = register_filesystem(basicfs_type);
	if(likely(ret == 0)) 
		printk(KERN_INFO "Basicfs: Registered");
	else
		printk(KERN_INFO "Basicfs: Failed to register\n");

	printk(KERN_INFO "Basicfs: initialized\n");
	return 0;
}

static void basicfs_exit(void)
{
	printk(KERN_INFO "Basicfs: unitializing...\n");
	
	int ret = 0;
	
	ret = unregister_filesystem(basicfs_type);
        if(likely(ret == 0))
                printk(KERN_INFO "Basicfs: Unregistered\n");
        else
                printk(KERN_INFO "Basicfs: Failed to unregister\n");	

	printk(KERN_INFO "Basicfs: unitialized\n");
}

module_init(basicfs_init);
module_exit(basicfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sahil Aggarwal <sahilagg0693@gmail.com>");
