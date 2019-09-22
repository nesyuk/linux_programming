#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/list_sort.h>

struct mydev {
    char *name;
    int num;
    struct list_head list;
};

static struct mydev *dev1;
static struct mydev *dev2;
static struct mydev *dev3;

static struct mydev first = {
    .name = "start",
    .num = 0,
    .list = LIST_HEAD_INIT(first.list) // assign each pointer (next, prev) inside the list field to point to that very field thus representing a list of a single element.
};

static struct mydev* init_mydev(char *name, int num){

    struct mydev *dev;
    dev = kmalloc(sizeof(struct mydev *), GFP_KERNEL);
    dev->name = name;
    dev->num = num;
    INIT_LIST_HEAD(&dev->list); // function to initialize the list
    return dev;
}

static int mydev_cmp(void *priv, struct list_head *a, struct list_head *b){

    struct mydev *d1;
    struct mydev *d2;
    
    d1 = list_entry(a, struct mydev, list);
    d2 = list_entry(b, struct mydev, list);

    return d1->num >= d2->num;
}

static void print_dev_list(struct list_head *head){

    struct mydev *dev = NULL;

    list_for_each_entry(dev, head, list){
        printk(KERN_INFO "dev name: %s\n", dev->name);
    }
}

static int __init my_init(void){
   
    LIST_HEAD(mydevlist); //declares a variable of type struct list_head and initializes it

    printk(KERN_INFO "init module\n");

    dev1 = init_mydev("first", 1);
    dev2 = init_mydev("second", 2);
    dev3 = init_mydev("third", 3);
    list_add(&first.list, &mydevlist);
    list_add(&dev1->list, &mydevlist);
    list_add(&dev2->list, &mydevlist);
    list_add(&dev3->list, &mydevlist);

    print_dev_list(&mydevlist); // prints in the backwards order: third, second, first, start
    list_sort(NULL, &mydevlist, mydev_cmp);
    print_dev_list(&mydevlist); // prints in sorted-by-num order

    return 0;
}


static void __exit my_exit(void){
    printk(KERN_INFO "exit module\n");

    kfree(dev1);
    kfree(dev2);
    kfree(dev3);
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
