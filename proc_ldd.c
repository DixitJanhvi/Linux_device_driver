#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Janhvi Dixit <dixitjanhvi26@gmail.com");
MODULE_DESCRIPTION("Basic read and write loadable module in /proc directory");

static struct proc_dir_entry *my_proc_dir_entry;

char msg[128];  // fixed safe buffer

static ssize_t my_read(struct file *file_pointer, char *user_space_buffer, size_t count, loff_t *offset)
{
    printk("my_read\n");

    size_t len = strlen(msg);
    int result;
    if (*offset >= len) // Becaasue cat command needs eod
    {
        return 0;
    }
    result = copy_to_user(user_space_buffer, msg, len);
    *offset += len;
    return len;
}

static ssize_t my_write(struct file *file_pointer, const char *user_space_buffer, size_t count, loff_t *offset)
{
    printk("my_write\n");
    size_t to_copy;
    int result;
   
    to_copy = min(count, sizeof(msg) - 1);
    result = copy_from_user(msg, user_space_buffer, to_copy);
    if (result != 0)
    {
        return -EFAULT;
    }
    msg[to_copy] = '\0'; 
    printk("Received from user: %s\n", msg);
    *offset += to_copy;
    return to_copy;
}

struct proc_ops my_proc_ops = {

    .proc_read = my_read,
    .proc_write = my_write

};

static int my_module_init(void)
{
    printk("my_module_init: entry\n");

    my_proc_dir_entry = proc_create("my_module",
                                    0,
                                    NULL,
                                    &my_proc_ops);

    if (my_proc_dir_entry == NULL)
    {
        printk("File is not created, error in init function");
        return -1;
    }

    printk("my_module_init: exit\n");
    return 0;
}

static void my_module_exit(void)
{
    printk("my_module_exit: entry\n");
    proc_remove(my_proc_dir_entry);

    printk("my_module_exit: exit\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
