#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/hrtimer.h>

static unsigned long delays[100];
static int num;

module_param_array_named(delays, delays, long, &num, 0644);

static int cur_delay = 0;
static long milli_to_nano = 1000000L;
static struct hrtimer timer;
static ktime_t expires;

static ktime_t get_timer_delay(void){

    return ktime_set(0, delays[cur_delay++]*milli_to_nano);
}

static enum hrtimer_restart timer_func(struct hrtimer *timer){
     
     printk(KERN_INFO "timer call: %d", cur_delay);

     if(cur_delay < num) 
     {
     	expires = get_timer_delay();
     	hrtimer_set_expires(timer, expires);         
     	return HRTIMER_RESTART;
     }

     return HRTIMER_NORESTART;
}

static int __init mydriver_init(void){

    printk(KERN_INFO "%d", num);
    
    //init
    hrtimer_init(&timer, CLOCK_REALTIME, HRTIMER_MODE_REL);

    // init ktime
    if (num > 0)
        expires = get_timer_delay();
    
    timer.function = timer_func;
    hrtimer_start(&timer, expires, HRTIMER_MODE_REL);

    return 0;
}

static void __exit mydriver_exit(void){

    hrtimer_cancel(&timer);
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
