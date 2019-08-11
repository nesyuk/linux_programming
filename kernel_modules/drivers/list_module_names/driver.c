#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/list_sort.h>
#include <linux/slab.h>
#include <linux/kobject.h>

#define MY_BUF_SIZE (ssize_t) ((2) * PAGE_SIZE)

static struct kobject *mydev_kobj;
static char *mydata;

static void get_module_names(void);

static ssize_t mydata_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	sprintf(buf, "%s", mydata);
        return strlen(mydata) + 1;
}

static ssize_t mydata_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t size){
	return size;
}

static struct kobj_attribute mydata_attrs = 
     __ATTR(mydata, 0444, mydata_show, mydata_store);

static struct attribute *attrs[] = {
	&mydata_attrs.attr,
	NULL,
};

static struct attribute_group attr_grp = {
	.attrs = attrs,
};

int my_cmp(void *priv, struct list_head *a, struct list_head *b){
    struct module *mod_a;
    struct module *mod_b;

    mod_a = list_entry(a, struct module, list);
    mod_b = list_entry(b, struct module, list);
    return strcmp(mod_a->name, mod_b->name);
}

static void save_module_names(struct list_head *head){
    struct list_head *pos;
    struct module *mod;
    list_for_each(pos, head){
        mod = list_entry(pos, struct module, list);
        sprintf(mydata + strlen(mydata), "%s\n", mod->name);
    }
}

static void get_module_names(void){
     struct list_head *head = THIS_MODULE->list.prev;
     list_sort(NULL, head, my_cmp);
     save_module_names(head);
}

static int __init mydev_init(void){

	printk(KERN_INFO "loading module");
	mydev_kobj = kobject_create_and_add("mydev", kernel_kobj);
	int retval = sysfs_create_group(mydev_kobj, &attr_grp);
        if(retval)
            kobject_put(mydev_kobj);
	mydata = kmalloc(MY_BUF_SIZE, GFP_KERNEL);
        get_module_names();

	return 0;
}

static void __exit mydev_exit(void){

	if(mydata)
		kfree(mydata);
	mydata = NULL;

	printk(KERN_INFO "unloading module");
	kobject_put(mydev_kobj);
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
