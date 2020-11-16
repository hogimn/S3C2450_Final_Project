#create device file
mknod /dev/mds2450_sg90 c 30 0

#arch/arm/mach-s3c2416/mach-mds2450.c
static struct platform_device mds2450_device_dht11 = { 
    .name = "mds2450_dht11"
};

static struct platform_device mds2450_device_sg90 = { 
    .name = "mds2450_sg90"
};

#drivers/char/Kconfig
config MDS2450_SG90
    tristate "MDS2450 SG90 driver"
    depends on MACH_MDS2450
    default y
    help
        mds2450 board sg90 driver

#drivers/char/Makefile
obj-$(CONFIG_MDS2450_SG90) += mds2450_sg90.o

#drivers/misc/Kconfig
config MDS2450_DHT11
    tristate "MDS2450 DHT11 driver"
    depends on MACH_MDS2450
    default y
    help
        mds2450 board dht11 driver

#drivers/misc/Makefile
obj-$(CONFIG_MDS2450_DHT11) += mds2450_dht11.o
