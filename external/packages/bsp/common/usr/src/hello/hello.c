#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void)
{
	printk("Hello Orange Pi -- init\n");

	return 0;
}
static void hello_exit(void)
{
	printk("Hello Orange Pi -- exit\n");

	return;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
