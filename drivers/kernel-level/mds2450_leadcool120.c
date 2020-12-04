/*
#include <linux/module.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <mach/gpio-nrs.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/pwm.h>

#include "mds2450_leadcool120.h"

#define DEVICE_NAME "mds2450_fan"
#define PRINTK_FAIL printk("%s[%d]: failed\n", __FUNCTION__, __LINE__)

static char fan_name[] = "mds2450_pwm_fan";

static struct pwm_device *pwm_dev;


static int FAN_open(struct inode *inode, struct file *filp);
static int FAN_release(struct inode *inode, struct file *filp);
static int FAN_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int FAN_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static int FAN_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static int FAN_open(struct inode *inode, struct file *filp){
	
	printk("Device has been opened...\n");
	//https://www.kernel.org/doc/html/latest/driver-api/miscellaneous.html#c.pwm_request
	//TOUT3이므로 3으로 지정
	pwm_dev = pwm_request(3, "pwm_dev");

	if(NULL == pwm_dev){
			PRINTK_FAIL;
			return -1;
	}
	
	return 0;
}
static int FAN_release(struct inode *inode, struct file *filp){
	printk("Device has been closed...\n");

	pwm_free(pwm_dev);
	return 0;	
}
static int FAN_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
//sk 예제 복붙
	char data[11];
	
	copy_from_user(data, buf, count);
	printk("data >>>>> = %s\n", data);

	return count;
}
static int FAN_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
//sk 예제 복붙
	char data[20] = "this is read func...";

	copy_to_user(buf, data, count);

	return 0;
}
static int FAN_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
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
			printk("command error");
			break;
	}
	
	return 0;
}

struct file_operations FAN_fops = {
	.open		= FAN_open,
	.release	= FAN_release,
	.write		= FAN_write,
	.read		= FAN_read,
	.unlocked_ioctl	= FAN_ioctl,
};

static int __init FAN_init(void){
	int ret;
	
	printk("fan module init");
	
	//GPB3을 TOUT으로 설정
	s3c_gpio_cfgpin(S3C2410_GPB(3), S3C_GPIO_SFN(2));
	
	
	//PWM Fan 전원이 인가되어도 최저로 돌기위해 pull down 설정
	s3c_gpio_setpull(S3C2410_GPB(3), S3C_GPIO_PULL_DOWN);
	
	//캐릭터 디바이스 등록
	ret = register_chrdev(PWM_FAN_MAJOR, fan_name, &FAN_fops);
	
	printk(KERN_DEBUG DEVICE_NAME " initialized\n");
	
	return ret;
}

static void __exit FAN_exit(void){
	printk("fan module exit");
	
	unregister_chrdev(PWM_FAN_MAJOR, fan_name);
	
    printk(KERN_DEBUG DEVICE_NAME " exited\n");
}

module_init(FAN_init);
module_exit(FAN_exit);
