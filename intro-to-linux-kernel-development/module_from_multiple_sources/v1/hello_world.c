#include <linux/module.h>
#include "kernel_greeter.h"

//The following external functions are placed into the header file
//extern void print_greet_message(void);
//extern void print_exit_message(void);

//when we declare the static identifier which means the function is local scope or means only available in this c file (will not called by the other c file.
static int __init hello_world_init(void)
{
	print_greet_message();
	
	return 0;
}

static void __exit hello_world_exit(void)
{
	print_exit_message();
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple LKM made up of multiple source files");
MODULE_AUTHOR("Janet Tsang");
