#include "Led.h"
#include "Status.h"

void init_LED(void)
{
    PORT10_IOCR0 &= ~((0x1F) << PC1);
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    PORT10_IOCR0 |= ((0x10) << PC1);
    PORT10_IOCR0 |= ((0x10) << PC2);
}

void Red_On()
{ // pcl 0, ps 1
    PORT10_OMR |= (1 << PS1);
    PORT10_OMR &= ~(1 << PCL1);
}

void Red_Off()
{ // pcl 1, ps 0
    PORT10_OMR &= ~(1 << PS1);
    PORT10_OMR |= (1 << PCL1);
}

void Blue_On()
{
    PORT10_OMR |= (1 << PS2);
    PORT10_OMR &= ~(1 << PCL2);
}

void Blue_Off()
{
    PORT10_OMR &= ~(1 << PS2);
    PORT10_OMR |= (1 << PCL2);
}

// status �� ���� led ����
void setting_LED(int status)
{
    if (status < 0 || status > 4)
    {
        //��� on
        Red_On();
        Blue_On();
        for (int i = 0; i < DELAY / 2; i++)
            ;
        Red_Off();
        Blue_Off();
        for (int i = 0; i < DELAY / 2; i++)
            ;
        Red_On();
        Blue_On();
        for (int i = 0; i < DELAY / 2; i++)
            ;
        Red_Off();
        Blue_Off();
    }
    else if (status == DOOR_LOCK)
    {
        Red_On();
        Blue_Off();
    }
    else if (status == DOOR_UNLOCK)
    {
        Red_Off();
        Blue_On();
    }
    else if (status == CAR_IN_UNLOCK)
    {
        Red_Off();
        Blue_On();
    }
    else if (status == CAR_IN_LOCK)
    {
        Red_On();
        Blue_Off();
    }
}

void blink_Red_LED(int term)
{
    Red_Off();
    for (int i = 0; i < DELAY / term; i++)
        ;
    Red_On();
    for (int i = 0; i < DELAY / 2; i++)
        ;
    Red_Off();
    for (int i = 0; i < DELAY / 2; i++)
        ;
    Red_On();
}

void blink_Blue_LED(int term)
{
    Blue_Off();
    for (int i = 0; i < DELAY / term; i++)
        ;
    Blue_On();
    for (int i = 0; i < DELAY / term; i++)
        ;
    Blue_Off();
    for (int i = 0; i < DELAY / term; i++)
        ;
    Blue_On();
}
