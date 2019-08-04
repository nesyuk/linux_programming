#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

#define MY_KOBJ "kobj_folder"
#define KBUF_SIZE (size_t) ((2) * PAGE_SIZE)

static char *my_kobj;
static struct kobject *driver_kobj;
static char *char_arr[10];
static unsigned int arr_size;
static char *msg;

module_param_array_named(my_chars, char_arr, charp, &arr_size, 0644);

static ssize_t mykobj_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    printk(KERN_INFO "show: %s", my_kobj);
    return sprintf(buf, "%s\n", my_kobj);
}

static ssize_t mykobj_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
   printk(KERN_INFO "store to : %s", my_kobj);
   sprintf(my_kobj, "%s", buf);
   return count;    
}

static struct kobj_attribute mykobj_attr = __ATTR(my_kobj, 0644, mykobj_show, mykobj_store);

static struct attribute *attrs[] = {
    &mykobj_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static int __init init_mydriver(void){
    int res;
    int i;

    my_kobj = kmalloc(KBUF_SIZE, GFP_KERNEL);

    printk(KERN_INFO "initialising...");
    driver_kobj = kobject_create_and_add(MY_KOBJ, kernel_kobj);
    if(!driver_kobj)
        return -ENOMEM;

    res = sysfs_create_group(driver_kobj, &attr_group);
    if(res)
        kobject_put(driver_kobj);

    msg = kmalloc(256, GFP_KERNEL);
    for (i = 0; i < arr_size; i++)
        strcat(strcat(msg, char_arr[i]), " ");

    sprintf(my_kobj, "%s", msg);
    kfree(msg);

    printk(KERN_INFO "initialised");
    return 0;
}

static void __exit exit_mydriver(void){

    kfree(my_kobj);
    kobject_put(driver_kobj);
    printk(KERN_INFO "release");
}

module_init(init_mydriver);
module_exit(exit_mydriver);

MODULE_LICENSE("GPL");
