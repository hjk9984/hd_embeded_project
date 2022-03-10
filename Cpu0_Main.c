#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "my_lib.h"
#include "Status.h"
#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;
unsigned int status = DOOR_LOCK;

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

    // CCU61 T12 100ms
    init_CCU6((myCCU6 *)&CCU61,100,0x0F);

    // ENTIRE SEQUENCE
    while(1){
        setting_LED(status);
        setting_RGB(status);
        if(status == DOOR_LOCK){
            if(Unlock())
                status = DOOR_UNLOCK;
        }
        else if(status == DOOR_UNLOCK){
            if(Parked()){
                status = CAR_IN_UNLOCK;     //      DOOR UNLOCKED   /   PARKED
                setting_RGB(status);
                /*do something*/
                status = CAR_IN_LOCK;       //      DOOR LOCKED     /   PARKED
                setting_RGB(status);
                while(1);
             }
        }
    }
    return (1);
}



