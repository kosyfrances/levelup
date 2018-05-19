#pragma once

// Linux Kernel
#include <linux/fs.h>

/**
 * SSCrypt module configuration and data.
 */
struct ssc_data {
  char *encryption_key;
};

/**
 * SSCrypt global module configuration data.
 */
extern struct ssc_data sscrypt;

/**
 * User process has opened the device file.
 */
static int ssc_device_open(struct inode *inode, struct file *file);

/**
 * User process has closed the device file.
 */
static int ssc_device_release(struct inode *inode, struct file *file);

/**
 * Handle a user process read from the device file.
 */
static ssize_t ssc_device_read(struct file *file, char *buffer, size_t length, loff_t *offset);

/**
 * Handle a user process write to the device file.
 */
static ssize_t ssc_device_write(struct file *file, const char *buffer, size_t length, loff_t *offset);
