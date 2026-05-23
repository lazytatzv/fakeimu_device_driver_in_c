#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simplechar"

static dev_t dev_num;
static struct cdev simple_cdev;
static struct class *simple_class;

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
  int ret;
  // major = register_chrdev()
  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

  if (ret < 0)
    return ret;
  
  printk(KERN_INFO "major = %d, minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

  cdev_init(&simple_cdev, &fops);
  
  ret = cdev_add(&simple_cdev, dev_num, 1);

  if (ret < 0)
    return ret;

  simple_class = class_create(DEVICE_NAME);

  if (IS_ERR(simple_class))
    return PTR_ERR(simple_class);
  
  device_create(
    simple_class,
    NULL,
    dev_num,
    NULL,
    DEVICE_NAME);

  printk(KERN_INFO "Successfully Initialized!");

  return 0;
}

static void __exit simple_exit(void) {
  device_destroy(simple_class, dev_num);
  class_destroy(simple_class);
  cdev_del(&simple_cdev);
  unregister_chrdev_region(dev_num, 1);


  //unregister_chrdev(major, DEVICE_NAME);
  printk(KERN_INFO "unloaded!");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modern version of simplechar");
