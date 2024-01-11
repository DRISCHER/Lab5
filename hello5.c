#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct execution_time {
    struct list_head list;
    ktime_t start_time;
    ktime_t end_time;
};

MODULE_AUTHOR("Andrii Stupak IO-13");
MODULE_DESCRIPTION("Lab 5 Zalikova 1325");
MODULE_LICENSE("GPL");

static LIST_HEAD(execution_list);

static uint q = 1;
module_param(q, uint, 0664);
MODULE_PARM_DESC(q, "Number of iterations");

static void __exit hello_exit(void); // Forward declaration of hello_exit

static int __init hello_init(void) {
    uint i;
    struct execution_time *new_entry;

    pr_info("Iteration count: %d\n", q);

    BUG_ON(q == 0);
    
    for (i = 0; i < q; i++) {
        new_entry = kmalloc(sizeof(*new_entry), GFP_KERNEL);

        if (!new_entry) {
            pr_err("Memory allocation failure!\n");
            hello_exit(); // Correct usage of hello_exit
            return -ENOMEM;
        }

        new_entry->start_time = ktime_get();
        pr_info("Hello, world sproba: %d\n", i + 1);
        new_entry->end_time = ktime_get();

        list_add_tail(&new_entry->list, &execution_list);
    }

    return 0;
}

static void __exit hello_exit(void) {
    struct execution_time *entry, *temp;

    list_for_each_entry_safe(entry, temp, &execution_list, list) {
        pr_info("Time taken: %lld ns",
        ktime_to_ns(entry->end_time - entry->start_time));

        list_del(&entry->list);
        kfree(entry);
    }

    if (!list_empty(&execution_list)) {
        pr_alert("Execution list not empty at module exit\n");
    }
}

module_init(hello_init);
module_exit(hello_exit);

