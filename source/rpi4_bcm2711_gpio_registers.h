/* Raspberry Pi4 Register View for GPIO */
/* BCM2611 ARM Peripherals */
/* Filename: ~/source/rpi4_bcm2711_gpio_registers.h */

#define GPIO_BASE 0xFE200000  // 0x7E20_0000 (legacy master address)

#define GPFSEL0 0x00 // Offset of the GPIO Function Select Register 0
#define GPFSEL1 0x04 // Offset of the GPIO Function Select Register 1
#define GPFSEL2 0x08 // Offset of the GPIO Function Select Register 2
#define GPFSEL3 0x0C // Offset of the GPIO Function Select Register 3
#define GPFSEL4 0x10 // Offset of the GPIO Function Select Register 4
#define GPFSEL5 0x14 // Offset of the GPIO Function Select Register 5

#define GPSET0 0x1C // Offset of the GPIO Set Register 0
#define GPSET1 0x20 // Offset of the GPIO Set Register 1

#define GPCLR0 0x28 // Offset of the GPIO Clear Register 0
#define GPCLR1 0x2C // Offset of the GPIO Clear Register 1

#define GPLEV0 0x34 // Offset of the GPIO Level Register 0
#define GPLEV1 0x38 // Offset of the GPIO Level Register 1
