#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_my_print(const char* message) {
    printk(message);
    return 0;
}
