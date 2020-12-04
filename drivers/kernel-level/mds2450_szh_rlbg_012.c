/*
	################################################
	#					RELAY					   #
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

#include "mds2450_szh_rlbg_012.h"

#define DEVICE_NAME "mds2450_relay"

static long relay_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	  if(_IOC_TYPE(cmd) != RELAY_MAGIC) return -EINVAL;
      if(_IOC_NR(cmd) >= RELAY_MAXNR) return -EINVAL;

	  switch(cmd) {
		case IN1_LOW	:{gpio_set_value(S3C2410_GPG(4), 0);break;}
		case IN1_HIGH	:{gpio_set_value(S3C2410_GPG(4), 1);break;}

		case IN2_LOW	:{gpio_set_value(S3C2410_GPG(5), 0);break;}
		case IN2_HIGH	:{gpio_set_value(S3C2410_GPG(5), 1);break;}

		case IN3_LOW	:{gpio_set_value(S3C2410_GPG(6), 0);break;}
		case IN3_HIGH	:{gpio_set_value(S3C2410_GPG(6), 1);break;}

		case IN4_LOW	:{gpio_set_value(S3C2410_GPG(7), 0);break;}
		case IN4_HIGH	:{gpio_set_value(S3C2410_GPG(7), 1);break;}		
		
		default : 
				break;
		}
		return 0;
}

static struct file_operations relay_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl		= relay_ioctl,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &relay_fops,
};

static int __devinit RELAY_probe(struct platform_device *pdev)
{

	s3c2410_gpio_cfgpin(S3C2410_GPG(4), S3C_GPIO_SFN(1));
	s3c2410_gpio_cfgpin(S3C2410_GPG(5), S3C_GPIO_SFN(1));
	s3c2410_gpio_cfgpin(S3C2410_GPG(6), S3C_GPIO_SFN(1));
	s3c2410_gpio_cfgpin(S3C2410_GPG(7), S3C_GPIO_SFN(1));

	s3c2410_gpio_setpin(S3C2410_GPG(4), 1);
	s3c2410_gpio_setpin(S3C2410_GPG(5), 1);
	s3c2410_gpio_setpin(S3C2410_GPG(6), 1);
	s3c2410_gpio_setpin(S3C2410_GPG(7), 1);	

	printk(DEVICE_NAME" probe\n");
    
	return 0;
}

static int __devexit RELAY_remove(struct platform_device *pdev)
{
   
	gpio_free(S3C2410_GPG(4));
	gpio_free(S3C2410_GPG(5));
	gpio_free(S3C2410_GPG(6));
	gpio_free(S3C2410_GPG(7));
	
	printk(DEVICE_NAME "removed\n");
	
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


static struct platform_driver RELAY_device_driver = {
    .probe      = RELAY_probe,
    .remove     = __devexit_p(RELAY_remove),
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    }
};

static int __init RELAY_init(void)
{
    int ret;
	
	misc_register(&misc);

    ret = platform_driver_register(&RELAY_device_driver);
    
	if(!ret){
        printk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        printk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&RELAY_device_driver);
    }
	
	printk(KERN_DEBUG DEVICE_NAME " initialized\n");
	
	 
    return ret;
}

static void __exit RELAY_exit(void)
{
	misc_deregister(&misc);
	platform_device_unregister(&pdev);
    platform_driver_unregister(&RELAY_device_driver);
    
	printk(KERN_DEBUG DEVICE_NAME " exited\n");
}

module_init(RELAY_init);
module_exit(RELAY_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hogimn@gmail.com");
