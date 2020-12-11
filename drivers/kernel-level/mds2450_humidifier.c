/*
	################################################
	#					humidifier					   #
	################################################
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <mach/gpio-nrs.h>
#include <mach/gpio-fns.h>
#include <asm/uaccess.h>
#include <asm/gpio.h>

#include "mds2450_humidifier.h"

#define DEVICE_NAME "mds2450_humidifier"

#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static long humidifier_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	  if(_IOC_TYPE(cmd) != HUMIDIFIER_MAGIC) return -EINVAL;
      if(_IOC_NR(cmd) >= HUMIDIFIER_MAXNR) return -EINVAL;

	  switch(cmd) {
		case HUMIDIFIER_LOW 	:{gpio_set_value(S3C2410_GPG(15), 0);break;}
		case HUMIDIFIER_HIGH	:{gpio_set_value(S3C2410_GPG(15), 1);break;}
		
		default : 
				break;
		}
		return 0;
}

static struct file_operations humidifier_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl		= humidifier_ioctl,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &humidifier_fops,
};

static int __devinit humidifier_probe(struct platform_device *pdev)
{

	s3c2410_gpio_cfgpin(S3C2410_GPG(15), S3C_GPIO_SFN(1));

	s3c2410_gpio_setpin(S3C2410_GPG(15), 1);

	printk(KERN_INFO "%s successfully loaded\n", DEVICE_NAME);
    
	return 0;
}

static int __devexit humidifier_remove(struct platform_device *pdev)
{
   
	gpio_free(S3C2410_GPG(15));
	
	printk(KERN_INFO "%s successfully removed\n", DEVICE_NAME);
	
    return 0;
}


static void release_pdev(struct device * dev){ 
    dev->parent     = NULL;
}

static struct platform_device pdev  =
{
    .name   = DEVICE_NAME,
    .id     = -1, 
    .dev    = { 
        .release    = release_pdev,
    },  
};


static struct platform_driver humidifier_device_driver = {
    .probe      = humidifier_probe,
    .remove     = __devexit_p(humidifier_remove),
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    }
};

static int __init humidifier_init(void)
{
    int ret;
	
	misc_register(&misc);

    ret = platform_driver_register(&humidifier_device_driver);
    
	if(!ret){
        gprintk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        gprintk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&humidifier_device_driver);
    }
	 
    return ret;
}

static void __exit humidifier_exit(void)
{
	misc_deregister(&misc);
	platform_device_unregister(&pdev);
    platform_driver_unregister(&humidifier_device_driver);
}

module_init(humidifier_init);
module_exit(humidifier_exit);

MODULE_LICENSE("GPL");