// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

#define MAX 10000

//Exercise let python programme can keep reading the current value of jiffies
/* Python code:
 *	import os
 *	fd = os.open("/proc/jiffies/value", os.O_RDONLY);
 *	os.read(fd, 100);
 * */

/*hint: /usr/local/src/linux/include/linux/proc_fs.h
 * cat /proc/version <- copied & modified from /usr/local/src/linux/fs/proc/version.c API call: proc_create_single()
 *now
 * cat /proc/devices <- which API called?? (devices.c)
 * */

static int jiffies_proc_show(struct seq_file *m, void *v)
{
	//dump_stack();
	seq_printf(m, "Jiffies = %8llu\n", get_jiffies_64());
	return 0;
}

static int jiffies_proc_ops_show(struct seq_file *f, void *v)
{
	//dump_stack();
	
	int i = *(loff_t *) v;
	if (i < MAX) {
		if (i == 0)
			seq_puts(f, "Jiffies values:\n");
		jiffies_proc_show(f, v);
	}
	return 0;
}

static void *jiffies_proc_ops_start(struct seq_file *f, loff_t *pos)
{
        if (*pos < MAX)
                return pos;
        return NULL;
}

static void *jiffies_proc_ops_next(struct seq_file *f, void *v, loff_t *pos)
{
        (*pos)++;
        if (*pos >= MAX	)
                return NULL;
        return pos;
}

static void jiffies_proc_ops_stop(struct seq_file *f, void *v)
{
        /* Nothing to do */
}

static const struct seq_operations jiffies_proc_ops = {
        .start = jiffies_proc_ops_start,
        .next  = jiffies_proc_ops_next,
        .stop  = jiffies_proc_ops_stop,
        .show  = jiffies_proc_ops_show
};

static int __init proc_jiffies_init(void)
{
	struct proc_dir_entry *pde;
	pde = proc_mkdir("jiffies", NULL);
	proc_create_seq("current", 0, pde, &jiffies_proc_ops);
	return 0;
}

static void __exit proc_jiffies_exit(void)
{
	remove_proc_subtree("jiffies", NULL);
}

module_init(proc_jiffies_init);
module_exit(proc_jiffies_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janet Tsang");
MODULE_DESCRIPTION("A simple kernel module to expose jiffies to user-space via /proc");
