// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/debugfs.h>
#include <linux/jiffies.h>

u32 count = 123;

static int __init jiffies_debugfs_init(void)
{
	struct dentry *de;

	de = debugfs_create_dir("jiffies", NULL);
	debugfs_create_u64("value", 0644, de, &jiffies_64);
	debugfs_create_u32("count", 0644, de, &count);
	
	return 0;
}

static void __exit jiffies_debugfs_exit(void)
{
	pr_notice("%s:%s: count = %u\n", KBUILD_MODNAME, __func__, count);
	debugfs_lookup_and_remove("jiffies", NULL); //this function will help us to lookup the entry pointer of this kthread and remove it. no need to send the struct dentry pointer here (NULL).
}

module_init(jiffies_debugfs_init);
module_exit(jiffies_debugfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janet Tsang");
MODULE_DESCRIPTION("A simple LKM to demonstrate exposing files under debugfs.");
