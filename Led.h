#ifndef LED_H_
#define LED_H_

#define PORT10_BASE (0xF003B000)
#define PORT10_IOCR0 (*(volatile unsigned int *)(PORT10_BASE + 0x10))
#define PORT10_OMR (*(volatile unsigned int *)(PORT10_BASE + 0x04))

#define PC1 11
#define PCL1 17
#define PS1 1

#define PC0 3
#define PC2 19
#define PCL2 18
#define PS2 2

#define PORT02_BASE (0xF003A200)
#define PORT02_IOCR0 (*(volatile unsigned int *)(PORT02_BASE + 0x10))
#define PORT02_IN (*(volatile unsigned int *)(PORT02_BASE + 0x24))

#define P0 0
#define P1 1

void init_LED(void);
void Red_On(void);
void Red_Off(void);
void Blue_On(void);
void Blue_Off(void);

void setting_LED(int);
void blink_Red_LED(int);
void blink_Blue_LED(int);

#endif /* LED_H_ */
