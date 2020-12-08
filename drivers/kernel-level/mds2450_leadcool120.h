#ifndef __MDS2450_PWM_FAN__
#define __MDS2450_PWM_FAN__

#include "mds2450_pwm.h"

// https://www.kernel.org/doc/html/latest/driver-api/miscellaneous.html#c.pwm_config
/*
https://blog.naver.com/divenire_am/220489050085
PWM FAN 주파수 권장 25kHz, 범위 21~28kHz
위 kernel 주소에 의하면 nano-seconds 단위이므로
PERIOD = 25 kHz로 설정
1/25 kHz = 40 usec
40 000 ns = 40 usec
duty또한 ns단위
0~40 usec = 0 ~ 40000 ns
*/

#define PWM_FAN_PERIOD		40000

#define PWM_FAN_SPEED(x)	(4000*((x>10)?(10):((x<0)?(0):(x))))

#define PWM_FAN_SPEED_00	0
#define PWM_FAN_SPEED_01	4000
#define PWM_FAN_SPEED_02	8000
#define PWM_FAN_SPEED_03	12000
#define PWM_FAN_SPEED_04	16000
#define PWM_FAN_SPEED_05	20000
#define PWM_FAN_SPEED_06	24000
#define PWM_FAN_SPEED_07	28000
#define PWM_FAN_SPEED_08	32000
#define PWM_FAN_SPEED_09	36000
#define PWM_FAN_SPEED_10	40000


// IOCTL 명령어 타입, 묶음 같은 의미나 큰 의미는 없다(?)
#define	PWM_FAN_IOCTL_MAGIC	'p'

#define PWM_FAN_ENABLE		_IO(PWM_FAN_IOCTL_MAGIC, 0)
#define PWM_FAN_DISABLE		_IO(PWM_FAN_IOCTL_MAGIC, 1)
#define PWM_FAN_DUTYRATE	_IOW(PWM_FAN_IOCTL_MAGIC, 2, struct pwm_duty_t)
// 최대 명령어 갯수, MAN number
#define PWM_FAN_IOCTL_MAXNR	3

#define PWM_FAN_PATH		"/dev/mds2450_leadcool120"

#endif
