#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

/*SYSCALL_DEFINE0(buffer_printk, const char* message)
{
    printk(message);
    return 0;
}*/

asmlinkage int sys_buffer_printk(const char* message) 
{
    printk(KERN_INFO "Hello world!!!!!!!!!!!!!!!!\n");
    char buf[] = KERN_INFO "%s\n";
    printk(buf, message);
    return 0;
}
