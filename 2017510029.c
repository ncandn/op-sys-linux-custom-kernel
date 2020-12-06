#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h> 
#include <linux/buffer_head.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/siginfo.h>
#include <linux/pid_namespace.h>
#include <linux/pid.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Building Custom Linux Kernel");
MODULE_AUTHOR("ncandikmen");
MODULE_VERSION("0.01");

// File Operation Functions to Reach proc/%s/cmdline   
struct file *file_open(const char *path, int flags, int rights) {
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

int file_close(struct file *file) {
    filp_close(file, NULL);
    return 0;
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
} 

struct file *f;

// Initializing task struct to approach tasks.
struct task_struct *task;

void arg_all(void){
	printk(KERN_INFO "%s","UID \t PPID \t PID \t NAME \t\t STATE \n");
    for_each_process( task ){
			printk(KERN_INFO "%d \t %d \t %d \t %s \t\t %ld \n", task->cred->uid, task_ppid_nr(task), task->pid, task->comm, task->state);
        }
}
void no_arg(void){
	printk(KERN_INFO "%s", "Right Usage:\n-all prints some information (process id and its argument/s) about all processes.\n");
	printk(KERN_INFO "%s", "-p takes process id and prints the details of it\n");
	printk(KERN_INFO "%s", "-k takes process id\n");
}
void arg_pid(char *pid){
	char buffer[100];
    char cmdf[50];
    snprintf(cmdf,sizeof(cmdf),"/proc/%s/cmdline",pid);
    f = file_open(cmdf, O_CREAT |  O_RDWR | O_APPEND, \
                                       S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,buffer,100);
    printk("pid : %s \t cmdline : %s\n",pid,buffer);
    file_close(f);
}
void arg_kid(char *pid){
	// Couldn't operate kill function.
	// printk(KERN_INFO "%s", "The task %s is killed.", pid);
}
SYSCALL_DEFINE2(mysyscalls,int,opt,char,*pid){
        if(opt == 0){
            no_arg();
        }
        else if(opt == 1){
            arg_all();
        }
        else if(opt == 2){
            arg_pid(pid);
        }
        else if(opt == 3){
            arg_kid(pid);
        }
        return 0;
}
