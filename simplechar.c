#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simplechar"

static int major;
static char msg[] = "hello, from kernel!\n";

static int dev_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "device opened!\n");
  return 0;
}

static ssize_t dev_read(
  struct file *file,
  char __user *buffer,
  size_t len,
  loff_t *offset) {
  int msg_len = strlen(msg);

  if (*offset >= msg_len)
    return 0;

  if (copy_to_user(buffer, msg, msg_len))
    return -EFAULT;

  *offset += msg_len;

  return msg_len;

}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = dev_open,
  .read = dev_read,
};

static int __init simple_init(void) {
  major = register_chrdev(0, DEVICE_NAME, &fops);
  printk(KERN_INFO "Successfully Initialized!\n");
  printk(KERN_INFO "major = %d\n", major);
  return 0;
}

static void __exit simple_exit(void) {
  unregister_chrdev(major, DEVICE_NAME);
  printk(KERN_INFO "unloaded!");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
