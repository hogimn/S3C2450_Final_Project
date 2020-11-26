#include <stdio.h>
#include <servo.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>

static int fd_servo;
static struct pwm_duty_t duty;

void servo_init(void)
{
    mknod(SERVO_DEV_PATH, S_IRWXU|S_IRWXG|S_IFCHR,
          MKDEV(SG90_PWM_MAJOR, 0));
    fd_servo = open(SERVO_DEV_PATH, O_RDWR);
    duty.period = SERVO_PWM_PERIOD;
    ioctl(fd_servo, SERVO_ENABLE);
}

void servo_rotate(int rotate_amount)
{
    duty.pulse_width = rotate_amount;
    ioctl(fd_servo, SERVO_DUTYRATE, &duty);
}

void servo_deinit(void)
{
    close(fd_servo);
    ioctl(fd_servo, SERVO_DISABLE);
}
