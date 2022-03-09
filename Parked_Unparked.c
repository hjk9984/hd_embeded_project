#include "Parked_Unparked.h"

void init_ultrasonic(void)
{
    /* Init TRIG Pin - P15.4 (Output) */
    PORT15_IOCR4 &= ~((0x1F) << PC4);
    PORT15_IOCR4 |= ((0x10) << PC4);

    /* Init ECHO Pin - P15.5 (Input) */
    PORT15_IOCR4 &= ~((0x1F) << PC5);
}

//interrupt init
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

    // interupt stop
    //********************************
    SCU_EICR2 &= ~(1 << FEN0);                   // Falling edge enable
    //********************************

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

// timer init
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

void interrupt_on(void){
    SCU_EICR2 |= (1 << FEN0);
}

int Parked(void){

    return 0;
}
