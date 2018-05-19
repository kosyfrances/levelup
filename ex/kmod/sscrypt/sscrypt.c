// Linux Kernel
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> //__init, __exit

// SSCrypt
#include "sscrypt.h"

// Provide module information.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandru Barbur");
MODULE_DESCRIPTION("A simple and secure encryption module.");
MODULE_VERSION("0.1");

// Instantiate the global module configuration data.
static struct ssc_data sscrypt;

/**
 * Symmetric encryption key parameter.
 */
module_param(sscrypt.encryption_key, charp, S_IWUGO);
MODULE_PARM_DESC(sscrypt.encryption_key, "The key to encrypt or decrypt messages with.");

/**
 * Initialize the kernel module.
 *
 * @returns returns 0 if successful
 */
static int __init ssc_init(void){
   printk(KERN_INFO "sscrypt: init\n");

   // done
   return 0;
}

/**
 * Clean up the kernel module.
 */
static void __exit ssc_exit(void){
   printk(KERN_INFO "sscrypt: exit\n");
}

// Register the module init and exit functions.
module_init(ssc_init);
module_exit(ssc_exit);
