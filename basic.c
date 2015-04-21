#include <linux/init.h>
#include <linux/module.h>

static int basicfs_init(void)
{
	printk(KERN_ALERT "Basicfs: initializing...\n");
	printk(KERN_ALERT "Basicfs: initialized\n");
	return 0;
}

static void basicfs_exit(void)
{
	printk(KERN_ALERT "Basicfs: unitializing...\n");
	printk(KERN_ALERT "Basicfs: unitialized\n");
}

module_init(basicfs_init);
module_exit(basicfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sahil Aggarwal <sahilagg0693@gmail.com>");
