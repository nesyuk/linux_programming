#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define MY_MAJOR 240
#define MY_MINOR 0
#define MYDEV_NAME "mycdev"
#define MYBUF_SIZE (ssize_t) ((2) * PAGE_SIZE)

#define IOC_MAGIC 't'
#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static struct cdev *mydev;
static dev_t devnum;
static unsigned int devcount = 1;

static char *mybuf;
static unsigned int mybuf_size = 0;

static unsigned int len_count = 0;
static unsigned int content_count = 0;

ssize_t mydev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos){

    unsigned err;
    unsigned copied;

    sprintf(mybuf, "%d %d", len_count, content_count);

    mybuf_size = strlen(mybuf) + 1;
    err = copy_to_user(buf, mybuf, mybuf_size);
    copied = mybuf_size - err;
    mybuf_size = 0;
    return copied;
}

ssize_t mydev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos){

    unsigned err = copy_from_user(mybuf, buf, mybuf_size);
    unsigned copied = mybuf_size - err;
    mybuf_size = 0;
    return copied;
}

int mydev_open(struct inode *inode, struct file *file){

    printk(KERN_INFO "driver is opened");
    return 0;
}

int mydev_release(struct inode *inode, struct file *file){

    printk(KERN_INFO "driver is closed");
    return 0;
}

long mydev_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    char *val;
    int len = 0;
    int err;

    val = kmalloc(30, GFP_KERNEL);
    err = copy_from_user(val, (char *) arg, 20);
    if (err)
        return -EINVAL;

    switch(cmd){
        case SUM_LENGTH:
            len_count += strlen(val);
            printk(KERN_INFO "len_count %d", len_count); 
            break;
        case SUM_CONTENT:
            err = kstrtoint(val, 10, &len);
            if(err)
                return -EINVAL;
            content_count += len;
            printk(KERN_INFO "content_count %d", content_count); 
            break;
        default:
            printk(KERN_INFO "IOCTL: unknown command: %u with argument: %ld", cmd, arg); break;
    }

    kfree(val);
    return arg;
}

const struct file_operations fops = {

    .owner = THIS_MODULE,
    .open = mydev_open,
    .write = mydev_write,
    .read = mydev_read,
    .release = mydev_release,
    .unlocked_ioctl = mydev_ioctl,
};

static int __init mydriver_init(void){

    printk(KERN_INFO "initialising module...");
    mydev = cdev_alloc();
    cdev_init(mydev, &fops);
    devnum = MKDEV(MY_MAJOR, MY_MINOR);
    register_chrdev_region(devnum, devcount, MYDEV_NAME);
    cdev_add(mydev, devnum, devcount++);
    mybuf = kmalloc(MYBUF_SIZE, GFP_KERNEL);
    
    printk(KERN_INFO "module is initialised");
    return 0;
}


static void  __exit mydriver_exit(void){

    printk(KERN_INFO "releasing module...");
    kfree(mybuf);
    unregister_chrdev_region(devnum, devcount--);
    cdev_del(mydev);

    printk(KERN_INFO "module is released");
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
