/*
	################################################
	#		HUMIDITY & TEMP. SENSOR : DHT11		   #
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

#define DEVICE_NAME "mds2450_dht11"
#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static unsigned char DHT11_read_byte(void)
{
    unsigned char DHT11_byte;
    int i;
    int delay_cnt;

    DHT11_byte = 0;

    for (i = 7; i >= 0; i--)
    {
        /* STEP5: block 50us until pin gets high */
        delay_cnt = 30;
        while (!(gpio_get_value(S3C2410_GPG(1))))
        {
            udelay(2);
            delay_cnt--;
            if (!delay_cnt)
                return -1;
        }

        /* 
         * STEP6: check if received data is bit "1" or "0"
         * bit "1" if pin gets high even after 26-28us
         * bit "0" if pin gets low after 26-28us
         */
        udelay(32);
        if (gpio_get_value(S3C2410_GPG(1)))
        {
            DHT11_byte |= (1 << i); 
        }

        /* delay until pin gets low */
        delay_cnt = 30;
        while (gpio_get_value(S3C2410_GPG(1)))
        {
            udelay(2);
            delay_cnt--;
            if (!delay_cnt)
            {
                return -1;
            }
        }
    }

    return DHT11_byte;
}

static ssize_t DHT11_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
    int i;
    int err = 0;
    int delay_cnt;
    int checksum;
    unsigned char databuf[5];

    memset(databuf, 0, sizeof(databuf));

    /*
     ******************************
     * MCU sends out start signal *
     ******************************
     */

    /* STEP1: clear pin and sleep 19msec */
    s3c2410_gpio_cfgpin(S3C2410_GPG(1), S3C_GPIO_SFN(1));
    s3c2410_gpio_setpin(S3C2410_GPG(1), 0);
    msleep(19);

    /* 
     * STEP2: set pin and change to input mode
     *        block 20-40us until pin gets low 
     */
    s3c2410_gpio_setpin(S3C2410_GPG(1), 1);
    s3c2410_gpio_cfgpin(S3C2410_GPG(1), S3C_GPIO_SFN(0));

    delay_cnt = 15;
    while (gpio_get_value(S3C2410_GPG(1)))
    {
        udelay(2);
        delay_cnt--;
        if (!delay_cnt)
        {
            err = 1;
            return -EFAULT;
        }
    }

    /*
     ***********************************
     * DHT11 sends out response signal *
     ***********************************
     */

    /* STEP3: block 80us until pin gets high */
    if (!err)
    {
        delay_cnt = 45;
        while (!(gpio_get_value(S3C2410_GPG(1))))
        {
            delay_cnt--;
            udelay(2);
            if (!delay_cnt)
            {
                err = 1;
                count = -EFAULT;
            }
        }
    }

    /* STEP4: block 80us until pin gets low */
    if (!err)
    {
        delay_cnt = 45;
        while ((gpio_get_value(S3C2410_GPG(1))))
        {
            delay_cnt--;
            udelay(2);
            if (!delay_cnt)
            {
                err = 1;
                count = -EFAULT;
            }
        }
    }

    /* 
     ***************************
     * DHT11 sends actual data *
     ***************************
     */
    
    if (!err)
    {
        for (i = 0; i < 5; i++)
        {
            databuf[i] = DHT11_read_byte();
        }

        checksum = 0 ;
        for (i = 0; i < 4; i++)
        {
            checksum += databuf[i];
        }

        /* checksum */
        if (checksum != databuf[4])
        {
            count = -EFAULT;
        }
        
        if (count != -EFAULT && copy_to_user(buf, databuf, count)) 
        {
            count = -EFAULT;
        }
    }
    s3c2410_gpio_cfgpin(S3C2410_GPG(1), S3C_GPIO_SFN(1));
    s3c2410_gpio_setpin(S3C2410_GPG(1), 1);
    return count;
}

static struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .read = DHT11_read,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
};

static int __devinit DHT11_probe(struct platform_device *pdev)
{

    s3c2410_gpio_cfgpin(S3C2410_GPG(1), S3C_GPIO_SFN(1));
    s3c2410_gpio_setpin(S3C2410_GPG(1), 1);

	printk(KERN_INFO "%s successfully loaded\n", DEVICE_NAME);
    return 0;
}

static int __devexit DHT11_remove(struct platform_device *pdev)
{
   
	gpio_free(S3C2410_GPG(1));
	
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


static struct platform_driver DHT11_device_driver = {
    .probe      = DHT11_probe,
    .remove     = __devexit_p(DHT11_remove),
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    }
};

static int __init DHT11_init(void)
{
    int ret;
	
	misc_register(&misc);

    ret = platform_driver_register(&DHT11_device_driver);
    
	if(!ret){
        gprintk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        gprintk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&DHT11_device_driver);
    }
	
	printk(KERN_DEBUG DEVICE_NAME " initialized\n");
	
	 
    return ret;
}

static void __exit DHT11_exit(void)
{
	misc_deregister(&misc);
	platform_device_unregister(&pdev);
    platform_driver_unregister(&DHT11_device_driver);
    
}

module_init(DHT11_init);
module_exit(DHT11_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hogimn@gmail.com");
