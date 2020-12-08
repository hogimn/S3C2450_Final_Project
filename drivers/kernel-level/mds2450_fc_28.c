/*
	################################################
	#	 	  SOIL MOISTURE SENSOR : FC_28		   #
	################################################
*/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>

#include <linux/io.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/workqueue.h> //work queue

#include <asm/uaccess.h> //user access
#include <linux/fs.h> //file_operatios 

#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include <mach/gpio.h>
#include <linux/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <mach/gpio-nrs.h>
#include <mach/gpio-fns.h>
#include <asm/gpio.h>


#define DEVICE_NAME "mds2450_fc_28"

#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static int soil_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	return gpio_get_value(S3C2410_GPG(2));
}

static struct file_operations soil_fops = {
    .owner = THIS_MODULE,
	.read =  soil_read,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &soil_fops,
};

static int __devinit soil_probe(struct platform_device *pdev)
{

	s3c_gpio_cfgpin(S3C2410_GPG(2), S3C_GPIO_SFN(0));

	printk(KERN_INFO "%s successfully loaded\n", DEVICE_NAME);
   
	return 0;
}

static int __devexit soil_remove(struct platform_device *pdev)
{
	gpio_free(S3C2410_GPG(2));
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


static struct platform_driver soil_device_driver = {
    .probe      = soil_probe,
    .remove     = __devexit_p(soil_remove),
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    }
};


static int __init soil_init(void)
{
	int ret;
	
	misc_register(&misc);
	
	ret = platform_driver_register(&soil_device_driver);
	
	if(!ret){
        gprintk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        gprintk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&soil_device_driver);
    }
	
	return 0;
}

static void __exit soil_exit(void)
{
	misc_deregister(&misc);
	platform_device_unregister(&pdev);
    platform_driver_unregister(&soil_device_driver);
}

module_init(soil_init);
module_exit(soil_exit);

MODULE_LICENSE("GPL");
