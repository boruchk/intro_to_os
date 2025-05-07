/*
	cs3224 - Spring 2025
	04/20/2025
	Lab8.c
	Boruch Khazanovich

	gcc lab8_test.c -o lab8_test

	sudo dmesg -C
	make
	sudo insmod lab8.ko
	
	./lab8_test <num_bytes>
	sudo rmmod lab8.ko
*/


#include <linux/init.h> 
#include <linux/module.h>
#include <linux/sched.h> 
#include <linux/miscdevice.h>
#include <linux/jiffies.h>
MODULE_LICENSE("Dual BSD/GPL"); 

#define MISC_DEV_NAME "lab8"
#define MESSAGE_BUF_SIZE 64

static unsigned long start_jiff = 0;
static unsigned long open_to_read_time = 0;
static bool first_read = true;



static int my_open(struct inode* in, struct file* fd) {
	// Tick value of the start of the module
	start_jiff = jiffies;

	printk(KERN_ALERT "my_open() started with jiffies value: %lu\n", start_jiff);
	return 0;
} 


static ssize_t my_read(struct file* fd, char __user* user_buf, size_t num_bytes, loff_t* offset) {
	printk(KERN_ALERT "---my_read() started---\n");
	
	if (first_read) {
		// Calculate the time since my_open() was called if this was the first read call
		open_to_read_time = ((jiffies- start_jiff) * 1000 / HZ);
		first_read = false;
	}
	
	char message_buf[MESSAGE_BUF_SIZE];
	sprintf(message_buf, "%lu\n%d", open_to_read_time, current->pid);
	size_t message_len = strlen(message_buf);
	printk(KERN_ALERT "open to read time: %lums\n pid: %d\n", open_to_read_time, current->pid);

	// Check if the entire file was already read
	if (*offset >= message_len) return 0;

	// Ensure that we dont read more than the size of the actual message to prevent buffer overflow
	size_t bytes_to_read = min(num_bytes, message_len - (size_t)*offset);

	// Read bytes_to_read characters from message_buf into user_buf
	if (copy_to_user(user_buf, message_buf + *offset, bytes_to_read) != 0) {
		printk(KERN_ALERT "error with copy_to_user()\n");
		return -EFAULT;
	}
	
	*offset += bytes_to_read;
	return bytes_to_read;
}



static struct file_operations my_fops = {.open = my_open, .read = my_read};
static struct miscdevice my_dev = {.minor = MISC_DYNAMIC_MINOR, .name = MISC_DEV_NAME, .fops = &my_fops};

static int my_init(void) {
	printk(KERN_ALERT "---starting driver---\n");

	if (misc_register(&my_dev) != 0) {
		printk(KERN_ALERT "Error registering my_dev\n");
		return 1;
	}

	printk(KERN_ALERT "virtual device %s got minor number: %i and was successfully registered\n", MISC_DEV_NAME, my_dev.minor);	
	return 0;
}


static void my_exit(void) {
	printk(KERN_ALERT "---exiting driver---\n");
	
	// Deregister my virtual device
	misc_deregister(&my_dev);
}



module_init(my_init);
module_exit(my_exit);
