#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
MODULE_PARM_DESC(howmany, "Number of times to print msg");
module_param(whom, charp, S_IRUGO);
MODULE_PARM_DESC(whom, "Whom the hello is addressed to");

// called when module is installed
int __init init_module()
{
	int i;

	for( i=0; i<howmany; i++)
		printk(KERN_ALERT "mymodule: Hello %s!\n", whom);

	return 0;
}


// called when module is removed
void __exit cleanup_module()
{
	int i;

	for( i=0; i<howmany; i++)
		printk(KERN_ALERT "mymodule: %d: Goodbye, cruel %s!!\n", i, whom);
}
