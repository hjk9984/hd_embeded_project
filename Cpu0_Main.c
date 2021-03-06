#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "my_lib.h"
#include "Status.h"
#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;
unsigned int status = DOOR_LOCK;

volatile int get_status() {
    return status;
}

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    init_LED();
    init_Switch();
    init_LED_RGB();
    init_GTM_TOM0_PWM_RGB();
    init_VADC();
    init_RGBLED();
    init_Buzzer();
    init_GTM_TOM0_PWM();

    GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK);     // buzzer pwm disable

    // CCU61 T12 100ms
    init_CCU6((myCCU6 *)&CCU61,100,0x0F);

    // ENTIRE SEQUENCE
    while(1){
        setting_LED(status);
        setting_RGB(status);
        if(status == DOOR_LOCK){
            disable_ultrasonic();
            if(Unlock())
                status = DOOR_UNLOCK;
        }
        else if(status == DOOR_UNLOCK){
            if(Parked()){
                status = CAR_IN_UNLOCK;     //      DOOR UNLOCKED   /   PARKED
                setting_RGB(status);
                /*delay*/
                for(int i=0;i<DELAY;i++);
                status = CAR_IN_LOCK;       //      DOOR LOCKED     /   PARKED
             }
        }
        else if(status == CAR_IN_LOCK){
            int tmp = Unlock();
            if(tmp == 1)
                status = CAR_IN_UNLOCK;
            else if(tmp == 2){
                for(int i=0; i < 5; i++){
                    note(0, 8);
                    for(int j=0; j<50000000; j++);
                }
                status = DOOR_LOCK;
            }
        }
        else if(status==CAR_IN_UNLOCK){
            if(Unparked()){
                status=DOOR_UNLOCK;
                /*delay*/
                for(int i=0;i<DELAY;i++);
                status=DOOR_LOCK;
            }
        }

    }
    return (1);
}



