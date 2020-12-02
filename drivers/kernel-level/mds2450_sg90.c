#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <mach/gpio-nrs.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/pwm.h>
#include "mds2450_sg90.h"

#define DEVICE_NAME "mds2450_sg90"
#define PRINTK_FAIL printk("%s[%d]: failed\n", __FUNCTION__, __LINE__)

static char servo_name[] = "mds2450_sg90";
static struct pwm_device *pwm_dev;

static long SG90_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;
    struct pwm_duty_t pwm_duty;

    if (_IOC_TYPE(cmd) != SG90_PWM_IOCTL_MAGIC ||
        _IOC_NR(cmd) >= SG90_PWM_IOCTL_MAXNR)
    {
        return -EINVAL; 
    }

    switch (cmd)
    {
        case SG90_PWM_ENABLE:
            pwm_enable(pwm_dev);
            printk("SG90_PWM_ENABLE\n");
            break;

        case SG90_PWM_DISABLE:
            pwm_disable(pwm_dev);
            printk("SG90_PWM_DISABLE\n");
            break;

        case SG90_PWM_DUTYRATE:
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
            
            printk("SG90_PWM_DUTYRATE\n");
            break;

        default:
            break;
    }

    return 0;
}

static int SG90_open(struct inode * inode, struct file * file)
{
    pwm_dev = pwm_request(2, "pwm_dev");
    if (NULL == pwm_dev)
    {
        PRINTK_FAIL;
        return -1;
    }

    return 0;
}

static int SG90_release(struct inode *inode, struct file *filp)
{
    pwm_free(pwm_dev);
    return 0;
}

static struct file_operations SG90_fops = {
    .owner   = THIS_MODULE,
    .open    = SG90_open,
    .release = SG90_release,
    .unlocked_ioctl = SG90_ioctl,
};

static int __devinit SG90_probe(struct platform_device *pdev)
{
    int ret;

    s3c_gpio_cfgpin(S3C2410_GPB(2), S3C_GPIO_SFN(2));
    s3c_gpio_setpull(S3C2410_GPB(2), S3C_GPIO_PULL_UP);

    ret = register_chrdev(SG90_PWM_MAJOR, servo_name, &SG90_fops);

    return ret;
}

static int __devexit SG90_remove(struct platform_device *pdev)
{
    unregister_chrdev(SG90_PWM_MAJOR, servo_name);
    return 0;
}

static struct platform_driver SG90_device_driver = {
    .probe      = SG90_probe,
    .remove     = __devexit_p(SG90_remove),
    .driver     = {
        .name   = "mds2450_sg90",
        .owner  = THIS_MODULE,
    }
};

static int __init SG90_init(void)
{
    int ret;

    ret = platform_driver_register(&SG90_device_driver);
    printk(KERN_DEBUG DEVICE_NAME " initialized\n");

    return ret;
}

static void __exit SG90_exit(void)
{
    platform_driver_unregister(&SG90_device_driver);
    printk(KERN_DEBUG DEVICE_NAME " exited\n");
}

module_init(SG90_init);
module_exit(SG90_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hogimn@gmail.com");
