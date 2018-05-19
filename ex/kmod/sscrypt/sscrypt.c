#include <linux/init.h> //__init, __exit
#include <linux/module.h>
#include <linux/kernel.h>

// Provide module information.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandru Barbur");
MODULE_DESCRIPTION("A simple and secure encryption module.");
MODULE_VERSION("0.1");

// TODO: parameters
static char *keyParam = "key";
module_param(keyParam, charp, S_IRUGO);
MODULE_PARM_DESC(keyParam, "The key to encrypt or decrypt messages with.");

/**
 * Initialize the kernel module.
 *
 * @returns returns 0 if successful
 */
static int __init sscrypt_init(void){
   printk(KERN_INFO "sscrypt: init\n");
   return 0;
}

/**
 * Clean up the kernel module.
 */
static void __exit sscrypt_exit(void){
   printk(KERN_INFO "sscrypt: exit\n");
}

// Register the module init and exit functions.
module_init(sscrypt_init);
module_exit(sscrypt_exit);
