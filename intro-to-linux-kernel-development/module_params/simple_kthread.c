#include <linux/module.h>
#include <linux/moduleparam.h> //used to define module params
#include <linux/kernel.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>

//Exercise 2: Implement functionality to change the kthread's name as listed in the 'ps ax' command listing via moodule parameters. (change /sys/module/simple_kthread/parameters/name doesn't work) hint: set_task_comm

struct task_struct *simple_kthread;

char thread_name[TASK_COMM_LEN] = KBUILD_MODNAME;
//module_param_string(name, thread_name, TASK_COMM_LEN, 0644);

static int change_name(const char *val, const struct kernel_param *kp)
{
	if(strlen(val) > 0 ){
		//char new_name[TASK_COMM_LEN] = val;
		//set_task_comm(simple_kthread, new_name);
		pr_notice("%s:%s: kthread name changed to %s\n", KBUILD_MODNAME, __func__, val);
	}else{
		set_task_comm(simple_kthread, thread_name);
		pr_warn("%s:%s: kthread name can't change\n", KBUILD_MODNAME, __func__);
	}	
	return 0;
}

module_param_call(name, change_name, param_get_string, &thread_name, 0644);
MODULE_PARM_DESC(name, "name of the thread as shown in ps listings...");

int count = 0;
module_param(count, int, 0444);
MODULE_PARM_DESC(count, "set the intial count value");

unsigned int delay = 5000;

static int change_delay(const char *val, const struct kernel_param *kp)
{
	
	unsigned int d;
	int ret = kstrtouint(val, 0, &d);
	
	if (ret) {
		pr_err("%s:%s: failed to convert %s to unsigned integer: %pe\n", KBUILD_MODNAME, __func__, val, ERR_PTR(ret)); //ERR_PTR <- Error to pointer
		return ret;
	}
	
	int validatedRet = param_set_uint_minmax(val, kp, 100, 5000);
	pr_notice("%s:%s: validated return value: %d\n", KBUILD_MODNAME, __func__, validatedRet);
	if (validatedRet){
		pr_err("%s:%s: delay(%s) not in defined range (100 to 5000): %pe\n", KBUILD_MODNAME, __func__, val, ERR_PTR(validatedRet));
		return validatedRet;
	}

	delay = d;
	pr_notice("%s:%s: delay changed to %d\n", KBUILD_MODNAME, __func__, d);
	return 0;
}

module_param_call(delay, change_delay, param_get_uint, &delay, 0644);
MODULE_PARM_DESC(delay, "set the sleep interval within the loop");

static int simple_kthread_fn(void *data)
{
	unsigned long timeout;

	while (!kthread_should_stop()) {
		pr_notice("%s:%s: counting %d\n", KBUILD_MODNAME, __func__, count);
		timeout = msleep_interruptible(delay);  
		if (timeout) {
			pr_warn("%s:%s: sleep cancelled (pending %lums)\n", KBUILD_MODNAME, __func__, timeout);
			break;
		}

		pr_info("%s:%s: woke up after sleep (%ums)\n", KBUILD_MODNAME, __func__, delay);
		++count;
	}
	return count;
}

static int __init simple_kthread_init(void)
{
	int ret = 0;
	
	simple_kthread = kthread_run(simple_kthread_fn, NULL, thread_name);
	if (IS_ERR(simple_kthread)) {
		pr_err("%s:%s: failed to launch a new kthread: %pe\n",
				KBUILD_MODNAME, __func__, simple_kthread);
	}else{
		pr_info("%s:%s: launched a new kthread (comm: %s, pid: %u)\n",
				KBUILD_MODNAME, __func__, thread_name, simple_kthread->pid);
	}
	
	return PTR_ERR_OR_ZERO(simple_kthread);
}

static void __exit simple_kthread_exit(void)
{
	pr_notice("%s:%s: Stopping kthread...", KBUILD_MODNAME, __func__);
	kthread_stop(simple_kthread);
}

module_init(simple_kthread_init);
module_exit(simple_kthread_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janet Tsang");
MODULE_DESCRIPTION("A simple LKM to demostrate usage of kthreads and module parameters");
