# Linux Device Driver

### Directory Structure
Directory | Explan.
----------|------------
***kernel-level*** | Platform Device Driver Code
***user-level*** | Library Wapper for Device Driver
***test-app*** | Sample App. to test each driver

### 1. kernel-level ###
To use platform device driver, add following lines of code to each file.    
1. arch/arm/mach-s3c2416/mach-mds2450.c
```c
static struct platform_device mds2450_device_dht11 = { 
    .name = "mds2450_dht11"
};

static struct platform_device mds2450_device_sg90 = { 
    .name = "mds2450_sg90"
};

static struct platform_device *mds2450_devices[] __initdata = { 

    ...

    &mds2450_device_dht11,
    &mds2450_device_sg90,
};
```

2. drivers/char/Kconfig
```
config MDS2450_SG90
    tristate "MDS2450 SG90 driver"
    depends on MACH_MDS2450
    default y
    help
        mds2450 board sg90 driver
```
3. drivers/char/Makefile
```
obj-$(CONFIG_MDS2450_SG90) += mds2450_sg90.o
```
4. drivers/misc/Kconfig
```
config MDS2450_DHT11
    tristate "MDS2450 DHT11 driver"
    depends on MACH_MDS2450
    default y
    help
        mds2450 board dht11 driver
```     
5. drivers/misc/Makefile
```
obj-$(CONFIG_MDS2450_DHT11) += mds2450_dht11.o
```
6. Build and install to kernel directory.
```
cd kernel-level
make
make install
```
7. Set each driver config to built-in[*].
```
make menuconfig    
```
8. Build linux kernel
```
make
```

### 2. user-level ###
1. Build static library
```
cd user-level
make
```

### 3. test-app ###
1. Build test application and deploy to target board via NFS.
```
cd test-app
make
make deploy
```
