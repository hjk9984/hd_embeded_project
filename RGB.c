#include "RGB.h"
#include "Status.h"
#include <stdio.h>

// red P2-pc7, blue P10-pc3, green P10-pc5

/* Initialize RGB LED */
void init_RGBLED(void)
{
    /* Reset IOCR0 bits */
    PORT02_IOCR4 &= ~((0x1F) << PC7);
    PORT10_IOCR4 &= ~((0x1F) << PC5);
    PORT10_IOCR0 &= ~((0x1F) << PC3);

    /* Set PC bits in IOCR0 with push-pull(2b10000) */
    PORT02_IOCR4 |= ((0x10) << PC7);
    PORT10_IOCR4 |= ((0x10) << PC5);
    PORT10_IOCR0 |= ((0x10) << PC3);
}

void RGB_RED_on()
{
    PORT02_OMR |= (1<<PS7);            // Set LED RED
    PORT10_OMR |= (1<<PCL5);           // Clear LED GREEN
    PORT10_OMR |= (1<<PCL3);           // Clear LED BLUE
}

void RGB_RED_off()
{
    PORT02_OMR |= (1<<PCL7);            // Clear LED RED
    PORT10_OMR |= (1<<PCL5);            // Clear LED GREEN
    PORT10_OMR |= (1<<PCL3);            // Clear LED BLUE
}

void RGB_BLUE_on()
{
    PORT02_OMR |= (1<<PCL7);            // Clear LED RED
    PORT10_OMR |= (1<<PCL5);            // Clear LED GREEN
    PORT10_OMR |= (1<<PS3);             // Set LED BLUE
}
void RGB_BLUE_off()
{
    PORT02_OMR |= (1<<PCL7);            // Clear LED RED
    PORT10_OMR |= (1<<PCL5);            // Clear LED GREEN
    PORT10_OMR |= (1<<PCL3);            // Clear LED BLUE
}

// status 에 따라 led 조절
void setting_RGB(int status){
    if (status < 0 || status > 4){

    }
    else if(status == DOOR_LOCK){
        RGB_RED_on();

    }
    else if(status == DOOR_UNLOCK){

    }
    else if(status == CAR_IN_UNLOCK){
        RGB_RED_on();

    }
    else if(status == CAR_IN_LOCK){
        RGB_BLUE_on();
    }
}
