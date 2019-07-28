#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static struct cdev * mydev;
static dev_t dev_num;
static unsigned int dev_count = 1;

#define MY_DEV "my_dev"
#define MYDEV_MAJOR 700
#define MYDEV_MINOR 0
#define MY_BUF_SIZE (size_t) ((2) * PAGE_SIZE)

struct mydev_data {

   char *data;
   int ptr; // current data pointer
   int end_ptr; // end data pointer
};

static loff_t mydev_llseek(struct file *f, loff_t ppos, int opt){

    loff_t temp_ppos;
    struct mydev_data * mydata = f->private_data;
    unsigned int position = ppos;

    switch(opt){
        case SEEK_SET: {
            temp_ppos = ppos;
            printk(KERN_INFO "llseek SEEK_SET(%d), position %lld", position, temp_ppos);
            break;
        }
        case SEEK_CUR: {
            temp_ppos = mydata->ptr + ppos;
            printk(KERN_INFO "llseek SEEK_CUR(%d), position: %lld", position, temp_ppos);
            break;
        }
        case SEEK_END: {
            temp_ppos = mydata->end_ptr + ppos;
            printk(KERN_INFO "llseek SEEK_END(%d), position: %lld", position, temp_ppos);
            break;
        }
        default:
            return EINVAL;
   }
   ppos = (temp_ppos < 0) ? 0 : temp_ppos;
   ppos = (ppos > mydata->end_ptr) ? mydata->end_ptr : ppos;

   position = ppos;
   printk(KERN_INFO "llseek to position: %d", position);
   mydata->ptr = ppos;

   return ppos;
}

static ssize_t mydev_read(struct file *f, char __user *buf, size_t size, loff_t *ppos){

    unsigned int err = 0;
    struct mydev_data *mydata;
    unsigned int position = *ppos;

    printk(KERN_INFO "reading from device");
    printk(KERN_INFO "reading from device from position: %d", position);
    mydata = f->private_data;
    printk(KERN_INFO "%s", mydata->data);
    
    // check validity of file position
    *ppos = *ppos < 0 ? 0 : *ppos;
    *ppos = *ppos > mydata->end_ptr ? mydata->end_ptr : *ppos;

    position = *ppos;
    printk(KERN_INFO "corrected position: %d", position);
    
    size = (size <= strlen(mydata->data)) ? size : strlen(mydata->data);
    err = copy_to_user(buf, mydata->data + mydata->ptr + *ppos, size);
    mydata->ptr += size - err;

    return size - err;
}

static ssize_t mydev_write(struct file *f, const char __user *buf, size_t size, loff_t *ppos){

    unsigned int err = 0;
    struct mydev_data *mydata;
    unsigned int position = *ppos;

    printk(KERN_INFO "writing to device on position: %d", position);
    mydata = f->private_data;

    // check validity of file position
    *ppos = *ppos < 0 ? 0 : *ppos;
    *ppos = *ppos > mydata->end_ptr ? mydata->end_ptr : *ppos;

    position = *ppos;
    printk(KERN_INFO "corrected position: %d", position);
    printk(KERN_INFO "data: %s", mydata->data + *ppos);
    
    err = copy_from_user(mydata->data + mydata->ptr + *ppos, buf, size);
    printk(KERN_INFO "new data: %s, new size: %zu", mydata->data, strlen(mydata->data));
    mydata->end_ptr = strlen(mydata->data) - 1;
    mydata->ptr += size - err;

    return size - err;
}

static int mydev_open(struct inode *inode, struct file *f){

    struct mydev_data * mydata;
    mydata = kmalloc(sizeof(struct mydev_data), GFP_KERNEL);
    mydata->data = kmalloc(MY_BUF_SIZE, GFP_KERNEL);
    mydata->ptr = 0;
    mydata->end_ptr = 0;
    sprintf(mydata->data, "%s", "first entry\n\0");
    f->private_data = mydata;
    dev_count++;

    printk(KERN_INFO "device opened");
    return 0;
}

static int mydev_release(struct inode *inode, struct file *f){

    struct mydev_data * mydata = f->private_data;
    kfree(mydata->data);
    kfree(mydata);
    f->private_data = NULL;

    printk(KERN_INFO "device closed");
    return 0;
}

static const struct file_operations fops = {
                   
        .owner   = THIS_MODULE,                                         
        .open    = mydev_open,                                 
        .release = mydev_release,                               
        .read    = mydev_read,                                   
        .write   = mydev_write,                                 
        .llseek  = mydev_llseek,
};

static int __init init_mydriver(void){

    mydev = cdev_alloc();
    cdev_init(mydev, &fops);
    dev_num = MKDEV(MYDEV_MAJOR, MYDEV_MINOR);   
    register_chrdev_region(dev_num, dev_count, MY_DEV);
    cdev_add(mydev, dev_num, dev_count);
    printk(KERN_INFO "init driver");
    return 0;
}

static void __exit cleanup_mydriver(void){

    cdev_del(mydev);
    unregister_chrdev_region(dev_num, dev_count);
    printk(KERN_INFO "exiting");
}

module_init(init_mydriver);
module_exit(cleanup_mydriver);

MODULE_LICENSE("GPL");
