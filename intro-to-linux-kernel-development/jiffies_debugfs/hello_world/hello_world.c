#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h> //let the function to asleep

//this variable will place to .init.data
char hello_msg[] __initdata = "Hello, world - from kernel-space.";

//reference: include/linux/init.h (marco of initdata, init, exit, etc.)
//__init.text -> optimizated the kernel - change the function run on text section in kernel -> used to free kernel memory after the hellow_world_init function is done
static int __init hello_world_init(void)
{
	pr_notice("%s:[%s:%d]:%s: %s\n", 
				KBUILD_MODNAME, __FILE__, __LINE__, __func__, hello_msg);
	dump_stack(); //dmesg will be strace the task of this kernel modules
	msleep_interruptible(30000);
	pr_info("%s:%s: returning out.\n", KBUILD_MODNAME, __func__);
	/*
	* printk(KERN_NOTICE "Hello world");
	* printk("<5>Hello world");
	*/	
	return 0;
	//return -1;
	//return 1;
}

//__exit -> .exit.text section -> no optimization all exit function of drivers grouped into one place
static void hello_world_exit(void)
{
	pr_info("%s:[%s:%d]:%s: Goodbye, cruel world.\n", 
				KBUILD_MODNAME, __FILE__, __LINE__, __func__);
	dump_stack();
	msleep_interruptible(30000);
	pr_info("%s:%s: returning out.\n", KBUILD_MODNAME, __func__);
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
//MODULE_LICENSE("Proprietary");

MODULE_DESCRIPTION("A simple hello-world module.");
MODULE_AUTHOR("Janet Tsang");

/* Try out the following changes:
	1. Try returning a negative number from hello_world_init()
		Ans: not allowed to insert module - insmod hello_world.ko not permiited
	1a. Try returning a positive number from hello_world_init()
		Ans: Can insert but dmesg showed the module is suspiciously returned 1 & called trace task

	2. Try changing the MODULE_LICENSE() from "GPL" to any other string like "Proprietary"
		Ans: dmesg showed that hello_world: module license 'Proprietary' taints kernel. && Disabling lock debugging due to kernel taint && hello_world: module license taints kernel.

	3. Try removing module_exit(), build, insert, remove and test.
		Ans: Removed module_exit(), allowed to compile, insert the module but cannot removed from the kernel! As the module doesn't know how to exist
*/
