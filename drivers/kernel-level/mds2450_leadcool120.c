/*
	################################################
	#		      FAN : LEADCOOL120				   #
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

#include "mds2450_leadcool120.h"

#define DEVICE_NAME "mds2450_leadcool120"
#define PRINTK_FAIL printk("%s[%d]: failed\n", __FUNCTION__, __LINE__)
#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static struct pwm_device *pwm_dev;

static int FAN_open(struct inode *inode, struct file *filp){
	
	gprintk("Device has been opened...\n");
	//https://www.kernel.org/doc/html/latest/driver-api/miscellaneous.html#c.pwm_request
	//TOUT3이므로 3으로 지정
	pwm_dev = pwm_request(3, "pwm_dev");

	if(NULL == pwm_dev){
			PRINTK_FAIL;
			return -1;
	}
	
	return 0;
}
static int FAN_release(struct inode *inode, struct file *filp)
{
	gprintk("Device has been closed...\n");
	pwm_free(pwm_dev);
	return 0;	
}

static long FAN_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret;	// 리턴값 변수
	struct pwm_duty_t pwm_duty;
	
	printk("call Fan\n");
/*
sk 예제에서
	if(_IOC_TYPE(cmd) != SK_MAGIC) return -EINVAL;
	if(_IOC_NR(cmd) >= SK_MAXNR) return -EINVAL;
	따로 정의하나, or처리로 묶어서 처리한다.
	명령어가 해당 타입인가?
	명령어가 해당 최대 명령어 숫자 이내인가?
*/	

	if(_IOC_TYPE(cmd) != PWM_FAN_IOCTL_MAGIC ||
		_IOC_NR(cmd) >= PWM_FAN_IOCTL_MAXNR){
	
		return -EINVAL; // errno.h에 정의, #define EINVAL 22
		//유효하지 않은 인수로 잘못된 인수 입력 시 프로그램에 반환 값이다.
	}

	switch(cmd){
		case PWM_FAN_ENABLE:
			pwm_enable(pwm_dev);
			printk("PWM_FAN_ENABLE\n");
			break;
		
		case PWM_FAN_DISABLE:
			pwm_disable(pwm_dev);
			printk("PWM_FAN_DISABLE\n");
			break;
		
		case PWM_FAN_DUTYRATE:
			// 유저 단에서 해당 값을 읽음
			ret = copy_from_user((void *)&pwm_duty, (const void *)arg, sizeof(struct pwm_duty_t));
			if(ret != 0){
				PRINTK_FAIL;
				return -1;
			}
			
			// 입력받은 값으로 pwm 설정
			ret = pwm_config(pwm_dev, pwm_duty.pulse_width, pwm_duty.period);
			if(ret < 0){
				PRINTK_FAIL;
				return -1;
			}
			
			printk("PWM_FAN_DUTYRATE %d\n", pwm_duty.pulse_width);
			break;
		
		
		default:
			printk("COMMAND ERROR\n");
			break;
	}
	
	return 0;
}

struct file_operations FAN_fops = {
	.open		= FAN_open,
	.release	= FAN_release,
	.unlocked_ioctl	= FAN_ioctl,
};

static struct miscdevice fan_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &FAN_fops,
};

static int __devinit FAN_probe(struct platform_device *pdev)
{
	//GPB3을 TOUT으로 설정
	s3c_gpio_cfgpin(S3C2410_GPB(3), S3C_GPIO_SFN(2));
	//PWM Fan 전원이 인가되어도 최저로 돌기위해 pull down 설정
	s3c_gpio_setpull(S3C2410_GPB(3), S3C_GPIO_PULL_DOWN);
	//캐릭터 디바이스 등록
	
	printk(KERN_INFO "%s successfully loaded\n", DEVICE_NAME);

    return 0;
}

static int __devexit FAN_remove(struct platform_device *pdev)
{
   gpio_free(S3C2410_GPB(3));
   
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


static struct platform_driver FAN_device_driver = {
    .driver     = {
        .name   = DEVICE_NAME,
        .owner  = THIS_MODULE,
    },
	.probe      = FAN_probe,
    .remove     = __devexit_p(FAN_remove),
    
};


static int __init FAN_init(void){
	
	int ret;
	
	misc_register(&fan_misc);
   
	ret = platform_driver_register(&FAN_device_driver);
	
	if(!ret){
        gprintk("platform_driver initiated  = %d \n", ret);
        ret = platform_device_register(&pdev);
        gprintk("platform_device_result = %d \n", ret);
        if(ret)
            platform_driver_unregister(&FAN_device_driver);
    }
	
	return ret;
}

static void __exit FAN_exit(void)
{
	misc_deregister(&fan_misc);
	
	platform_device_unregister(&pdev);
    platform_driver_unregister(&FAN_device_driver);
}

module_init(FAN_init);
module_exit(FAN_exit);

MODULE_LICENSE("GPL");