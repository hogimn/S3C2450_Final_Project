#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>


#define DRV_NAME "keyint"

#define OFF 0
#define ON  1

static int status=OFF;

static irqreturn_t keyinterrupt_func(int irq, void *dev_id, struct pt_regs *regs)
{
//	printk("\nkeypad was pressed \n");
	
	gpio_set_value(S3C2410_GPG(7),0);
	status=OFF;
	
	s3c_gpio_cfgpin(S3C2410_GPB(2), S3C_GPIO_SFN(1));
	gpio_set_value(S3C2410_GPB(3),1);
	s3c_gpio_cfgpin(S3C2410_GPB(2), S3C_GPIO_SFN(2));
/*
	// SEt LED ON/OFF
	switch(status)
	{
		case OFF : {gpio_set_value(S3C2410_GPG(7),1);status=ON;break;}
		case ON  : {gpio_set_value(S3C2410_GPG(7),0);status=OFF;break;} 
	}
*/
	return IRQ_HANDLED;
}

static int __init keyint_init(void)
{
	int ret;
	// set Interrupt mode
	s3c_gpio_cfgpin(S3C2410_GPG(3), S3C_GPIO_SFN(2));

	// set LED OUTPUT
	s3c_gpio_cfgpin(S3C2410_GPG(7), S3C_GPIO_SFN(1));
	gpio_set_value(S3C2410_GPG(7),0);

	if( request_irq(IRQ_EINT11, (void *)keyinterrupt_func,IRQF_DISABLED|IRQF_TRIGGER_FALLING, DRV_NAME, NULL) )
	{
		printk("failed to request external interrupt.\n");
		ret = -ENOENT;
		return ret;
	}
	printk(KERN_INFO "%s successfully loaded\npress sw14\n", DRV_NAME);
	return 0;
}

static void __exit keyint_exit(void)
{
	free_irq(IRQ_EINT11, NULL);
	printk(KERN_INFO "%s successfully removed\n", DRV_NAME);
}

module_init(keyint_init);
module_exit(keyint_exit);

MODULE_LICENSE("GPL");
