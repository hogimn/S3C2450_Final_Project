#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include "mds2450_led_onboard.h"

static char led_name[] = "mds2450_led_onboard";

static long 
mds2450_led_ioctl(struct file *file, 
                  unsigned int cmd, 
                  unsigned long arg)
{
    int val;

    if (_IOC_TYPE(cmd) != LED_ONBOARD_MAGIC ||
        _IOC_NR(cmd) >= LED_ONBOARD_MAXNR)
    {
        return -EINVAL;
    }

    switch (cmd)
    {
        case LED_ONBOARD_OFF:
            gpio_set_value(S3C2410_GPG(arg), 1);
            break;
        case LED_ONBOARD_ON:
            gpio_set_value(S3C2410_GPG(arg), 0);
            break;
        case LED_ONBOARD_TOGGLE:
            val = gpio_get_value(S3C2410_GPG(arg));
            val ^= 1;
            gpio_set_value(S3C2410_GPG(arg), val);
        default:
            break;
    }
    return 0;
}

static int
mds2450_led_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "led_open called\n");
        
    /* led off */
    gpio_set_value(S3C2410_GPG(4), 1);
    gpio_set_value(S3C2410_GPG(5), 1);
    gpio_set_value(S3C2410_GPG(6), 1);
    gpio_set_value(S3C2410_GPG(7), 1);
    
    /* mode to output */
    s3c_gpio_cfgpin(S3C2410_GPG(4), S3C_GPIO_SFN(1));
    s3c_gpio_cfgpin(S3C2410_GPG(5), S3C_GPIO_SFN(1));
    s3c_gpio_cfgpin(S3C2410_GPG(6), S3C_GPIO_SFN(1));
    s3c_gpio_cfgpin(S3C2410_GPG(7), S3C_GPIO_SFN(1));

    /* no pull */
    s3c_gpio_setpull(S3C2410_GPG(4), S3C_GPIO_PULL_NONE);
    s3c_gpio_setpull(S3C2410_GPG(5), S3C_GPIO_PULL_NONE);
    s3c_gpio_setpull(S3C2410_GPG(6), S3C_GPIO_PULL_NONE);
    s3c_gpio_setpull(S3C2410_GPG(7), S3C_GPIO_PULL_NONE);

    return 0;
}

static int
mds2450_led_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "led_release called\n");

    /* led off */
    gpio_set_value(S3C2410_GPG(4), 1);
    gpio_set_value(S3C2410_GPG(5), 1);
    gpio_set_value(S3C2410_GPG(6), 1);
    gpio_set_value(S3C2410_GPG(7), 1);

    return 0;
}

static struct file_operations mds2450_led_fops =
{
    .owner   = THIS_MODULE,
    .open    = mds2450_led_open,
    .release = mds2450_led_release,
    .unlocked_ioctl = mds2450_led_ioctl,
};

static int __devinit
mds2450_led_probe(struct platform_device *pdev)
{
    int ret;

    printk("led_probe called\n");

    ret = register_chrdev(LED_ONBOARD_MAJOR, 
                          led_name, 
                          &mds2450_led_fops);

    return ret;
}

static int __devexit
mds2450_led_remove(struct platform_device *pdev)
{
    printk("led_remove called\n");
    unregister_chrdev(LED_ONBOARD_MAJOR, led_name);

    return 0;
}

static struct platform_driver mds2450_led_device_driver = 
{
    .probe      = mds2450_led_probe,
    .remove     = __devexit_p(mds2450_led_remove),
    .driver     = {
        .name   = "mds2450_led_onboard",
        .owner  = THIS_MODULE,
    }
};

static int
__init mds2450_led_init(void)
{
    int ret;

    printk("led_init called\n");
    ret = platform_driver_register(&mds2450_led_device_driver);

    return ret;
}

static void
__exit mds2450_led_exit(void)
{
    printk("led called\n");
    platform_driver_unregister(&mds2450_led_device_driver);
}

module_init(mds2450_led_init);
module_exit(mds2450_led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hogimn@gmail.com");
