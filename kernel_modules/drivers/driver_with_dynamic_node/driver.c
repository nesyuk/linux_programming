#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define MYDEV_NAME "mycdev"
#define MYDEV_CLASS_NAME "mycdev_class"
#define MY_BUF_SIZE (ssize_t) ((2) * PAGE_SIZE)

static unsigned int MY_MINOR = 0;
static unsigned int MY_MAJOR = 0;

static unsigned int dev_count = 1;
static struct cdev * mydev;
static dev_t device_number;
struct class *mycls;
static char *mydev_buffer;
static int mydev_buffer_size;

ssize_t mydev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos){
    int err = copy_to_user(buf, mydev_buffer, mydev_buffer_size);
    int size = mydev_buffer_size - err;
    mydev_buffer_size = 0;
    return size;
}

ssize_t mydev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos){
    int err = copy_from_user(mydev_buffer, buf, lbuf);
    *ppos += lbuf - err;        
    return lbuf - err;
}

int mydev_open(struct inode *node, struct file *file){
    printk(KERN_INFO "Opening device %d...", MY_MAJOR);
    mydev_buffer = kmalloc(MY_BUF_SIZE, GFP_KERNEL);
    sprintf(mydev_buffer, "%d\n", MY_MAJOR);
    mydev_buffer_size = strlen(mydev_buffer);
    return 0;
}

int mydev_release(struct inode * node, struct file *file){
    printk(KERN_INFO "Releasing device ...");
    kfree(mydev_buffer);
    return 0;
}


const struct file_operations fops = {
     .owner = THIS_MODULE,
     .open = mydev_open,
     .read = mydev_read,
     .write = mydev_write,
     .release = mydev_release
};

static int __init mydev_init(void){

    printk(KERN_INFO "loading module ...");

    // register MAJOR-MINOR NUMBER region for device (when MAJOR = 0 and MINOR = 0, number is generated automatically)
    device_number = MKDEV(0, 0);
    alloc_chrdev_region(&device_number, MY_MINOR, dev_count, MYDEV_NAME);
    
    printk(KERN_INFO "got MAJOR number %d", MAJOR(device_number));
    MY_MAJOR = MAJOR(device_number);

    // allocate device structure
    mydev = cdev_alloc();

    // add new device (bind with device number)
    cdev_add(mydev, device_number, dev_count++);

    // initialize file operations for /dev/{MYDEV_NAME}
    cdev_init(mydev, &fops);

    // add device class in /dev/ 
    mycls = class_create(THIS_MODULE, MYDEV_CLASS_NAME);
    device_create(mycls, NULL, device_number, "%s", MYDEV_NAME);    
   
    printk(KERN_INFO "loaded!");
    return 0;
}

static void __exit mydev_exit(void){

    printk(KERN_INFO "unloading module ...");

    // remove device
    cdev_del(mydev);

    // unregister region
    unregister_chrdev_region(device_number, dev_count);

    // remove device class in /dev/
    device_destroy(mycls, device_number);
    class_destroy(mycls);

    printk(KERN_INFO "unloaded!");
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
