// SSCrypt
#include "sscrypt.h"

// User process has opened the device file.
static int ssc_device_open(struct inode *inode, struct file *file) {
  return 0;
}

// User process has closed the device file.
static int ssc_device_release(struct inode *inode, struct file *file) {
  return 0;
}

// Handle a user process read from the device file.
static ssize_t ssc_device_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
  return 0;
}

// Handle a user process write to the device file.
static ssize_t ssc_device_write(struct file *file, const char *buffer, size_t length, loff_t *offset) {
  return 0;
}
