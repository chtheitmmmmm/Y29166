#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/slab.h>

#include  "memento.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("COMP Student");
MODULE_DESCRIPTION("Hello driver!");

static int num = 1;		// number of minor.
module_param(num, int, S_IRUGO);

#define MAX_NUM 10

static char datas[MAX_NUM] = {0};		// values
static char origin_datas[MAX_NUM] = {0};	// origin values
static struct cdev * memento_cdevs[MAX_NUM] = {NULL}; // all devices
static dev_t dev_no;

static int min_max(int min, int value, int max) {
	return value > min ? value < max ? value : max : min;
}

// get minor number form file struct.
static int get_minor(struct file * file) {
	return MINOR(file->f_inode->i_rdev);
}


int memento_open(struct inode * inode, struct file * file) {
	return 0;
}

int memento_release(struct inode * inode, struct file * file) {
	return 0;
}

ssize_t memento_read(struct file * file, char __user * buf, size_t size, loff_t * ppos) {
	int minor = get_minor(file);
	int i, ret = 0;
	for (i = 0; i < size; ++i) {
		ret = copy_to_user(buf + i, datas + minor, 1);
		if (ret)
			return ret;
	}
	return ret;
}

ssize_t memento_write(struct file * file, const char __user * buf, size_t size, loff_t * ppos) {
	int minor = get_minor(file);
	int ret;
	char mybuf[1];
	if (size > 0) {
		ret = copy_from_user(mybuf, buf + size - 1, 1);
		if (ret)
			return ret;
		datas[minor] = mybuf[0];
	}
 	return 0;
}

static long memento_ioctl(struct file * file, unsigned int cmd, unsigned long arg) {
	int minor = get_minor(file);
	int ret = 0;
	switch (cmd) {
		case MOMENTO_RESET:
			datas[minor] = origin_datas[minor];
			break;
		case MOMENTO_GET:
			ret = copy_to_user((char __user *)arg, origin_datas + minor, 1);
			break;
		case MOMENTO_SET:
			ret = copy_from_user(origin_datas + minor, (char __user *)arg, 1);
			break;
		default:
			return -ENOTTY;
	}
	return ret;
}


static const struct file_operations memento_ops = {
	.owner          = THIS_MODULE,
	.open           = memento_open,
	.read           = memento_read,
	.write          = memento_write,
	.release        = memento_release,
	.unlocked_ioctl = memento_ioctl,
};

static int __init memento_init(void) {
	int i;
	num = min_max(1, num, 10);
	alloc_chrdev_region(&dev_no, 0, num, "memento");
	for (i = 0; i < num; ++i) {
		memento_cdevs[i] = cdev_alloc();
		cdev_init(memento_cdevs[i], &memento_ops);
		cdev_add(memento_cdevs[i], dev_no, i + 1);
		datas[i] = i;
		origin_datas[i] = i;
	}
	return 0;
}

static void __exit memento_exit(void) {
	int i;
	for (i = 0; i < num; ++i) {
		cdev_del(memento_cdevs[i]);
	}
	unregister_chrdev_region(dev_no, num);
}

module_init(memento_init);
module_exit(memento_exit);


