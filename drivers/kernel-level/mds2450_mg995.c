/*
	################################################
	#		      SERVO MOTOR : MG995			   #
	################################################
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>

#include <linux/pwm.h>

#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include "mds2450_mg995.h"

#define DEVICE_NAME "mds2450_mg995"
#define PRINTK_FAIL printk("%s[%d]: failed\n", __FUNCTION__, __LINE__)
#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static struct pwm_device *pwm_dev;

static int MG995_open(struct inode * inode, struct file * filp)
{
	gprintk("Device has been opened\n");
    
	pwm_dev = pwm_request(2, "pwm_dev");
    if(NULL == pwm_dev)
    {
        PRINTK_FAIL;
        return -1;
    }

    return 0;
}

static int MG995_release(struct inode *inode, struct file *filp)
{
	gprintk("Device has been closed...\n");
    pwm_free(pwm_dev);
    return 0;
}


static long MG995_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;
    struct pwm_duty_t pwm_duty;
	
	printk("call MG995\n");
    if (_IOC_TYPE(cmd) != MG995_PWM_IOCTL_MAGIC ||
        _IOC_NR(cmd) >= MG995_PWM_IOCTL_MAXNR)
    {
        return -EINVAL; 
    }

    switch (cmd)
    {
        case MG995_PWM_ENABLE:
            pwm_enable(pwm_dev);
            printk("MG995_PWM_ENABLE\n");
            break;

        case MG995_PWM_DISABLE:
            pwm_disable(pwm_dev);
            printk("MG995_PWM_DISABLE\n");
            break;

        case MG995_PWM_DUTYRATE:
            ret = copy_from_user((void *)&pwm_duty, (const void *)arg, sizeof(struct pwm_duty_t));
            if (ret != 0)
            {
                PRINTK_FAIL;
                return -1;
            }

            ret = pwm_config(pwm_dev, pwm_duty.pulse_width, pwm_duty.period);
            if (ret < 0)
            {
                PRINTK_FAIL;
                return -1;
            }
            
            printk("MG995_PWM_DUTYRATE\n");
            break;

        default:
			printk("COMMAND ERROR\n");
            break;
    }

    return 0;
}

static struct file_operations MG995_fops = {
    .open    = MG995_open,
    .release = MG995_release,
    .unlocked_ioctl = MG995_ioctl,
};

static struct miscdevice mg995_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &MG995_fops,
};

static int __devinit MG995_probe(struct platform_device *pdev)
{
    s3c_gpio_cfgpin(S3C2410_GPB(2), S3C_GPIO_SFN(2));
    s3c_gpio_setpull(S3C2410_GPB(2), S3C_GPIO_PULL_UP);
	
	printk(KERN_INFO "%s successfully loaded\n", DEVICE_NAME);

    return 0;
}

static int __devexit MG995_remove(struct platform_device *pdev)
{
   gpio_free(S3C2410_GPB(2));
   
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


static struct platform_driver MG995_device_driver = {
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    },
	.probe      = MG995_probe,
    .remove     = __devexit_p(MG995_remove),
    
};

static int __init MG995_init(void)
{
    int ret;
	
	printk(KERN_DEBUG DEVICE_NAME " initialized\n");
	
	misc_register(&mg995_misc);
	
    ret = platform_driver_register(&MG995_device_driver);
	
	if(!ret){
        gprintk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        gprintk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&MG995_device_driver);
    }

    return 0;
}

static void __exit MG995_exit(void)
{	
	misc_deregister(&mg995_misc);
	
	platform_device_unregister(&pdev);
    platform_driver_unregister(&MG995_device_driver);

}

module_init(MG995_init);
module_exit(MG995_exit);

MODULE_LICENSE("GPL");