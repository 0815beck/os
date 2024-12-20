/*
*   This kernel module adds a new entry /proc/seconds to the pseudo-file-system /proc
*   Reading /proc/seconds reports the number of seconds since the module was loaded
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/param.h>
#include <linux/jiffies.h>


#define PROC_NAME "seconds"
#define BUFFER_SIZE 128

ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos);

unsigned long start_jiffies;

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

int running_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "Module Running was loaded");
    start_jiffies = jiffies;
    printk(KERN_INFO "We have that start_jiffies = %lu", start_jiffies);
    return 0;
}

void running_exit(void) {
    printk(KERN_INFO "The module running was removed.");
    remove_proc_entry(PROC_NAME, NULL);
}

ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos) {

    printk(KERN_INFO "proc_read was called. HZ = %d, jiffies = %lu", HZ, jiffies);
    
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    unsigned long seconds = (jiffies - start_jiffies) / HZ;
    char buffer[BUFFER_SIZE];
    int rv;
    
    rv = sprintf(buffer, "%lu", seconds);
    copy_to_user(user_buf, buffer, rv);
    
    return rv;
}

module_init(running_init);
module_exit(running_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Running Module");
MODULE_AUTHOR("Nico");