#include <linux/init.h> 
#include <linux/module.h>
#include <linux/sched.h> 
MODULE_LICENSE("Dual BSD/GPL"); 

static unsigned long start_jiff;
static unsigned long end_jiff;
static ktime_t start_time;
static ktime_t end_time;

static int hello_init(void) {
	printk(KERN_ALERT "Hello, world\n");

	// Tick value of the start of the module
	start_jiff = jiffies;
	// Save the current time
	start_time = ktime_get_boottime();

	printk(KERN_ALERT "The tick value when the module started is: %lu\n", start_jiff);

	return 0;
} 

static void hello_exit(void) {
	printk(KERN_ALERT "Goodbye, cruel world\n");

	end_jiff = jiffies;
	end_time = ktime_get_boottime();

	// Calculate the difference of start tick to current tick and start time to current time
	long jiff_diff = (long)end_jiff - (long)start_jiff;
	ktime_t boottime_diff = end_time - start_time;

	printk(KERN_ALERT "The runtime of the module - calculated using jiffies - is: %ld ms\n", jiff_diff);	
	printk(KERN_ALERT "The runtime of the module - calculated using time - is: %lld ms\n", ktime_to_ms(boottime_diff));

}

module_init(hello_init);
module_exit(hello_exit); 
