#include "PWM_Buzzer.h"

void init_Buzzer(void)
{
    PORT02_IOCR0 &= ~((0x1F) << PC3);           // PORT02.3 : Alternate output function 1 (push-pull)
    PORT02_IOCR0 |= ((0x11) << PC3);            // PORT02.3 : GTM_TOUT3
}

void init_GTM_TOM0_PWM(void)
{
    /* GTM Enable */
    // Password Access to unlock WDTCPU0CON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    // Modify Access to clear ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    GTM_CLC &= ~(1 << DISR);                        // Enable GTM Module

    // Password Access to unlock WDTCPU0CON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    // Modify Access to set ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    while((GTM_CLC & (1 << DISS)) != 0);            // Wait until module is enabled

    /* GTM Clock Setting */
    GTM_CMU_FXCLK_CTRL &= ~((0xF) << FXCLK_SEL);    // Input clock of CMU_FXCLK : CMU_GCLK_EN

    GTM_CMU_CLK_EN &= ~((0x3) << EN_FXCLK);         // Enable all CMU_FXCLK
    GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK);

    /* GTM TOM0 PWM Setting */
    GTM_TOM0_TGC1_GLB_CTRL &= ~((0x3) << UPEN_CTRL3);   // TOM0 channel 11 enable update of
    GTM_TOM0_TGC1_GLB_CTRL |= ((0x2) << UPEN_CTRL3);    // register CM0, CM1, CLK_SRC

    GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << FUPD_CTRL3);  // Enable force update of TOM0 channel 11
    GTM_TOM0_TGC1_FUPD_CTRL |= ((0x2) << FUPD_CTRL3);
    GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << RSTCN0_CH3);  // Reset CN0 of TOM0 channel 11 on force update
    GTM_TOM0_TGC1_FUPD_CTRL |= ((0x2) << RSTCN0_CH3);

    GTM_TOM0_TGC1_ENDIS_CTRL &= ~((0x3) << ENDIS_CTRL3); // Enable channel 11 on an update trigger
    GTM_TOM0_TGC1_ENDIS_CTRL |= ((0x2) << ENDIS_CTRL3);
    GTM_TOM0_TGC1_OUTEN_CTRL &= ~((0x3) << OUTEN_CTRL3); // Enable channel 11 output on an update trigger
    GTM_TOM0_TGC1_OUTEN_CTRL |= ((0x2) << OUTEN_CTRL3);

    GTM_TOM0_CH11_CTRL |= (1 << SL);                    // High signal level for duty cycle

    GTM_TOM0_CH11_CTRL &= ~((0x7) << CLK_SRC_SR);       // Clock source : CMU_FXCLK(1) = 6250 kHz
    GTM_TOM0_CH11_CTRL |= (1 << CLK_SRC_SR);
    GTM_TOM0_CH11_SR0 = 12500 - 1;                      // PWM freq. = 6250 kHz / 12500 = 500 Hz
    GTM_TOM0_CH11_SR1 = 0;                              // Duty cycle = 0

    GTM_TOUTSEL0 &= ~((0x3) << SEL3);                   // TOUT3 : TOM0 channel 11

    GTM_TOM0_TGC1_GLB_CTRL |= (1 << HOST_TRIG);         // Trigger request signal to update
}

void set_pd(int pd)
{
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    // Modify Access to clear ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);



    PORT02_PDR0 &= ~((0x07) << 12);
    PORT02_PDR0 |=  ((pd) << 12);


    // Password Access to unlock WDTCPU0CON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    // Modify Access to set ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    while((GTM_CLC & (1 << DISS)) != 0);            // Wait until module is enabled
}

void note(int tone, int delay){
    int tones[] = {261, 277, 294, 311, 330, 349, 370, 392};
    volatile int pwm_cnt = PWM_FREQ / tones[tone];

    GTM_TOM0_CH11_SR0 = pwm_cnt - 1;
    GTM_TOM0_CH11_SR1 = pwm_cnt/2 - 1;

    //set_pd(1);

    GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK);      // enable

    for(int j = 0 ; j < 50000000 / delay ; j++) ;

    GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK);     // disable
}
