#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <my_lib.h>

/* SCU Registers */
#define SCU_BASE            (0xF0036000)
#define SCU_WDTSCON0        (*(volatile unsigned int*)(SCU_BASE + 0x0F0))
#define SCU_WDT_CPU0CON0    (*(volatile unsigned int*)(SCU_BASE + 0x100))
#define SCU_EICR2           (*(volatile unsigned int*)(SCU_BASE + 0x218))
#define SCU_IGCR0           (*(volatile unsigned int*)(SCU_BASE + 0x22C))

#define LCK                 1
#define ENDINIT             0
#define INP0                12
#define EIEN0               11
#define REN0                9
#define FEN0                8
#define EXIS0               4
#define IGP0                14

/* SRC Registers */
#define SRC_BASE            (0xF0038000)
#define SRC_CCU60_SR0       (*(volatile unsigned int*)(SRC_BASE + 0x420))
#define SRC_SCUERU0         (*(volatile unsigned int*)(SRC_BASE + 0xCD4))

#define TOS                 11
#define SRE                 10
#define SRPN                0

/* CCU60 Registers */
#define CCU60_BASE          (0xF0002A00)
#define CCU60_CLC           (*(volatile unsigned int*)(CCU60_BASE + 0x00))
#define CCU60_T12           (*(volatile unsigned int*)(CCU60_BASE + 0x20))
#define CCU60_T12PR         (*(volatile unsigned int*)(CCU60_BASE + 0x24))
#define CCU60_TCTR0         (*(volatile unsigned int*)(CCU60_BASE + 0x70))
#define CCU60_TCTR4         (*(volatile unsigned int*)(CCU60_BASE + 0x78))
#define CCU60_INP           (*(volatile unsigned int*)(CCU60_BASE + 0xAC))
#define CCU60_IEN           (*(volatile unsigned int*)(CCU60_BASE + 0xB0))

#define DISS                1
#define DISR                0
#define CTM                 7
#define T12PRE              3
#define T12CLK              0
#define T12STR              6
#define T12RS               1
#define INPT12              10
#define ENT12PM             7

/* Define PORT15 Registers for Ultrasonic */
#define PORT15_BASE     (0xF003B500)
#define PORT15_OMR      (*(volatile unsigned int*)(PORT15_BASE + 0x04))
#define PORT15_IOCR4    (*(volatile unsigned int*)(PORT15_BASE + 0x14))
#define PORT15_IN       (*(volatile unsigned int*)(PORT15_BASE + 0x24))

#define PCL4            20
#define PS4             4
#define PC5             11
#define PC4             3
#define P5              5

/* Function Prototype */
void init_ultrasonic(void);
void init_ERU(void);
void init_CCU60(void);

volatile unsigned int timer_cnt;
volatile unsigned int start_time;
volatile unsigned int end_time;
volatile unsigned int interval_time;
volatile unsigned int distance;
volatile unsigned char irq_ultra_sensor;

// ����
//volatile unsigned int LOCK = 1;
#define LOCK 1
#define SYSTEM_TIMER_0_31_0 *(volatile unsigned int *)(0xF0000000 + 0x10)

volatile unsigned int speed;
volatile unsigned int systemtick;
volatile unsigned int cycle;
volatile unsigned int distance_prev;
volatile unsigned int distance_curr;

#define PORT10_BASE     (0xF003B000)
#define PORT10_IOCR0    (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR      (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PORT02_BASE     (0xF003A200)
#define PORT02_IOCR0    (*(volatile unsigned int*)(PORT02_BASE + 0x10))
#define PORT02_IN       (*(volatile unsigned int*)(PORT02_BASE + 0x24))

#define PC1             11
#define PC2             19
#define PS1             1
#define PS2             2
#define PCL1            17
#define PCL2            18

volatile int get_distance(){
    return distance;
}

int Parked(void)
{
    init_LED();
//    init_Switch();
    init_VADC();
    PORT10_OMR |= (1<<PCL1);
    if (LOCK == 1)
    {
        irq_ultra_sensor = 0;
        /* Initialization */
        init_ultrasonic();
        init_ERU();
        init_CCU60();
        while(1)
        {
            irq_ultra_sensor = 0;
            while( irq_ultra_sensor == 0 );
            distance_prev = distance;
            systemtick = SYSTEM_TIMER_0_31_0;
            for(cycle = 0; cycle < 4166666 ; cycle++);     // 1s
            systemtick = SYSTEM_TIMER_0_31_0 - systemtick;
            distance_curr = distance;
            speed = abs(distance_prev - distance_curr);

            for (int i=0;i<10;i++){
                irq_ultra_sensor = 0;
                while( irq_ultra_sensor == 0 );
            }

            if (distance < 10 & speed < 1)
                return 1;
        }
    }
}

int Unparked(void)
{
    init_LED();
//    init_Switch();
    init_VADC();
    PORT10_OMR |= (1<<PCL1);
    if (LOCK == 1)
    {
        irq_ultra_sensor = 0;
        /* Initialization */
        init_ultrasonic();
        init_ERU();
        init_CCU60();
        while(1)
        {
            irq_ultra_sensor = 0;
            while( irq_ultra_sensor == 0 );
            distance_prev = distance;
            systemtick = SYSTEM_TIMER_0_31_0;
            for(cycle = 0; cycle < 4166666 ; cycle++);     // 1s
            systemtick = SYSTEM_TIMER_0_31_0 - systemtick;
            distance_curr = distance;
            speed = abs(distance_prev - distance_curr);

            for (int i=0;i<10;i++){
                irq_ultra_sensor = 0;
                while( irq_ultra_sensor == 0 );
            }

            if (distance > 50)
                return 1;
        }
    }
}

__interrupt( 0x0A ) __vector_table( 0 )
void CCU60_T12_ISR(void)
{
    timer_cnt++;
    if(timer_cnt == 1)
    {
        /* Set TRIG Pin */
        PORT15_OMR |= (1<<PS4);
    }
    else if(timer_cnt == 2)
    {
        /* Clear TRIG Pin */
        /* Generate 10us Pulse */
        PORT15_OMR |= (1<<PCL4);
    }
    else if(timer_cnt == 10000)
    {
        /* TRIG Period: 100ms */
        timer_cnt = 0;
    }
}

__interrupt( 0x0B ) __vector_table( 0 )
void ERU0_ISR(void)
{
    if((PORT15_IN & (1<<P5)) == 0)              // Falling edge
    {
        /* Get distance */
        end_time = timer_cnt;

        // 10us
        interval_time = end_time - start_time;  // clock per 0.02us

        // 340m/s -> 340_00cm/s -> 340_00
        // 1us    -> 1000000/340_00 = 29
        // ToF(Time of Flight) = distance * 2
        // distance = ToF us / 58 cm
        // distance = ToF 10us / 5.8 cm
        // distance = ToF 10us 17/100 cm
        //distance = ((interval_time/2)*34000)/1000_00;    // cm
        distance = (interval_time*17)/100;      // cm
        irq_ultra_sensor = 1;
    }
    else                                        // Rising edge
    {
        start_time = timer_cnt;
    }
}

void init_ultrasonic(void)
{
    /* Init TRIG Pin - P15.4 (Output) */
    PORT15_IOCR4 &= ~((0x1F) << PC4);
    PORT15_IOCR4 |= ((0x10) << PC4);

    /* Init ECHO Pin - P15.5 (Input) */
    PORT15_IOCR4 &= ~((0x1F) << PC5);
}

void init_ERU(void)
{
    /* ERU Input Channel 4 Setting */
    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDTSCON0 & (1 << LCK)) != 0);

    // Modify Access to clear ENDINIT bit
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) & ~ (1 << ENDINIT);
    while((SCU_WDTSCON0 & (1 << LCK)) == 0);

    SCU_EICR2 &= ~((0x7) << EXIS0);             // External input 3 is selected
    SCU_EICR2 |= ((0x3) << EXIS0);
    SCU_EICR2 |= (1 << REN0);                   // Rising edge enable
    SCU_EICR2 |= (1 << FEN0);                   // Falling edge enable
    SCU_EICR2 |= (1 << EIEN0);                  // The trigger event is enabled

    SCU_EICR2 &= ~((0x7) << INP0);              // An event from input ETL 4 triggers output OGU 0
    SCU_IGCR0 &= ~((0x3) << IGP0);              // IOUT(0) is activated in response to a trigger event
    SCU_IGCR0 |= (1 << IGP0);                   // The pattern is not considered

    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDTSCON0 & (1 << LCK)) != 0);

    /* Modify Access to set ENDINIT bit */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDTSCON0 & (1 << LCK)) == 0);

    /* SRC Interrupt Setting For ECU */
    SRC_SCUERU0 &= ~((0xFF) << SRPN);           // Set Priority : 0x0B
    SRC_SCUERU0 |= ((0x0B) << SRPN);

    SRC_SCUERU0 &= ~((0x3) << TOS);             // CPU0 services
    SRC_SCUERU0 |= (1 << SRE);                  // Service Request is enabled
}

void init_CCU60(void)
{
    /* CCU60 T12 Setting */

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~ (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    CCU60_CLC &= ~(1 << DISR);                  // Enable CCU60 Module

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    while((CCU60_CLC & (1 << DISS)) != 0);       // Wait until module is enabled

    CCU60_TCTR0 &= ~((0x7) << T12CLK);           // f_T12 = f_CCU6 / prescaler
    CCU60_TCTR0 |= (1 << T12CLK);                // F_CCU6 = 100 MHz, prescaler = 2
    CCU60_TCTR0 &= ~(1 << T12PRE);               // f_T12 = 50 MHz

    CCU60_TCTR0 &= ~(1 << CTM);                  // T12 always counts up and continues counting
                                                 // from zero after reaching the period value

    CCU60_T12PR = 500 - 1;                       // Interrupt freq. = f_T12 / (period value + 1)
    CCU60_TCTR4 |= (1 << T12STR);                // Interrupt freq. = 100 kHz (10 us)
    CCU60_T12 = 0;                               // Clear T12 counting value

    /* CCU60 T12 Interrupt Setting */
    CCU60_INP &= ~((0x3) << INPT12);             // Service Request output SR0 is selected
    CCU60_IEN |= (1 << ENT12PM);                 // Enable Interrupt for T12 Period-Match

    /* SRC Interrupt Setting For CCU60 */
    SRC_CCU60_SR0 &= ~((0xFF) << SRPN);           // Set Priority : 0x0A
    SRC_CCU60_SR0 |= ((0x0A) << SRPN);

    SRC_CCU60_SR0 &= ~((0x3) << TOS);             // CPU0 services

    SRC_CCU60_SR0 |= (1 << SRE);                  // Service Request is enabled

    /* CCU60 T12 Start */
    CCU60_TCTR4 = (1 << T12RS);                  // T12 starts counting
}
