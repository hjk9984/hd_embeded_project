#ifndef RGB_H_
#define RGB_H_


#define PORT10_BASE (0xF003B000)
#define PORT10_IOCR0 (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PC1 11
#define PCL1 17
#define PS1 1

#define PC0 3
#define PC2 19
#define PCL2 18
#define PS2 2

#define PORT02_BASE (0xF003A200)
#define PORT02_IOCR0 (*(volatile unsigned int*)(PORT02_BASE + 0x10))
#define PORT02_IN (*(volatile unsigned int*)(PORT02_BASE + 0x24))
#define PORT02_OMR (*(volatile unsigned int*)(PORT02_BASE + 0x04))

#define P0 0
#define P1 1
/* Define PORT02/10 Registers for RGB LED */
#define PORT02_BASE     (0xF003A200)
#define PORT02_IOCR4    (*(volatile unsigned int*)(PORT02_BASE + 0x14))
#define PORT02_OMR      (*(volatile unsigned int*)(PORT02_BASE + 0x04))

#define PC7             27
#define PCL7            23
#define PS7             7

#define PORT10_BASE     (0xF003B000)
#define PORT10_IOCR4    (*(volatile unsigned int*)(PORT10_BASE + 0x14))
#define PORT10_IOCR0    (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR      (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PC5             11
#define PC3             27
#define PCL5            21
#define PCL3            19
#define PS5             5
#define PS3             3



void init_RGBLED(void);


#endif /* RGB_H_ */
