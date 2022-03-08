#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "my_lib.h"
#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;


volatile unsigned int systick;
volatile unsigned int systick_curr;
volatile unsigned int systick_prev;
volatile unsigned int potential_meter;
volatile unsigned int potential_meter_duty;

volatile unsigned int red_duty;
volatile unsigned int green_duty;
volatile unsigned int blue_duty;

volatile unsigned int SW1_curr;
volatile unsigned int SW1_prev;
volatile unsigned int SW1_cnt;
volatile unsigned int SW1_debounce;
volatile unsigned int SW1_debounce_prev;

volatile unsigned int SW2_curr;
volatile unsigned int SW2_prev;
volatile unsigned int SW2_cnt;
volatile unsigned int SW2_debounce;
volatile unsigned int SW2_debounce_prev;

volatile unsigned int SW_state_curr;
volatile unsigned int SW_state_prev;
volatile unsigned int SW_state_cnt;
volatile unsigned int SW_state_debounce;

volatile unsigned char irq_timer;

//
volatile unsigned int stack[5]={0,};
volatile unsigned int stack_cnt;

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

    potential_meter_duty = 0;

    red_duty = 0;
    green_duty = 0;
    blue_duty = 0;

    SW1_curr = 0;
    SW1_prev = 0;
    SW1_cnt = 0;
    SW1_debounce = 0;
    SW2_curr = 0;
    SW2_prev = 0;
    SW2_cnt = 0;
    SW2_debounce = 0;
    SW_state_curr = 0;
    SW_state_prev = 0;
    SW_state_cnt = 0;
    SW_state_debounce = 0;
    stack_cnt = 0;

    irq_timer = 0;

    // CCU61 T12 100ms
    init_CCU6((myCCU6 *)&CCU61,100,0x0F);

    while(1)
    {
        //Default
        // 100ms
        irq_timer = 0;
        SW1_debounce_prev=SW1_debounce;
        SW2_debounce_prev=SW2_debounce;
        while( irq_timer == 0 );

        if(SW1_debounce_prev!=SW1_debounce | SW2_debounce_prev!=SW2_debounce)
            stack_cnt++;

        SW_state_prev = SW_state_curr;

        SW_state_curr = (SW2_debounce<<1) | (SW1_debounce<<0);          // [sw2_debounce,sw1_debounce]

        if(SW_state_curr!=SW_state_prev)
            SW_state_cnt=0;
        else if(SW_state_cnt<5)
            SW_state_cnt++;
        else{
            SW_state_debounce=SW_state_curr;
        }

        if( SW_state_debounce != 0 )                            // 뭔가 눌려서 debounce에 변화가 생기면
        {

            if     ( SW_state_debounce == 0x01 )                // SW1 is pushed,
            {
                stack[stack_cnt/2]=1;
                PORT10_OMR = (1<<PS2)   ;                       // LED BLUE on

            }
            else if( SW_state_debounce == 0x02 )                // SW2 is pushed, RGB.red
            {
                stack[stack_cnt/2]=0;
//                stack_cnt+=irq_timer;
                PORT10_OMR = (1<<PS2)   ;                       // LED BLUE on
            }
            else                                                // 둘다 눌리면 error로 간주, stack초기화, default로...
            {
                stack_cnt=0;
                for(volatile int i =0;i<5;i++)
                    stack[i]=0;

                // 틀리면 두번 RED blink
                PORT10_OMR = (1<<PCL1);                         // LED RED  off
                PORT10_OMR = (1<<PS1)  ;                        // LED RED  on
                PORT10_OMR = (1<<PS1) ;                         // LED RED  on
            }
        }
        else
        // 아무것도 눌리지 않은 상태
        // RED LED만 켜 놔라
        {
            PORT10_OMR = (1<<PCL2);                         // LED BLUE  off
            PORT10_OMR = (1<<PC1);
            GTM_TOM0_CH2_SR0 = 12500 - 1;
            GTM_TOM0_CH2_SR1 = potential_meter_duty;
        }


        // stack_cnt==8이면 stack과 비교
        if(stack_cnt==8){
            if(check_pwd(stack)){
                // 만약 일치하면 BLUE LED on상태, while문 break
                PORT10_OMR = (1<<PS2)   ;                       // LED BLUE on
                break;
            }
            else{
                //비교한게 틀렸으면 두번 RED blink, stack_cnt초기화, stack초기화
                PORT10_OMR = (1<<PS1)   ;                       // LED RED  on
                stack_cnt=0;
                for(unsigned int i=0;i<5;i++){
                    stack[i]=0;
                PORT10_OMR = (1<<PCL1)   ;                       // LED RED  on
                }
            }
        }
    }
    while(1){

    }
    return (1);
}


// 100ms timer
__interrupt( 0x0F ) __vector_table( 0 )
void CCU61_T12_ISR(void)
{
    // 여기선 interrupt를 SW1,2로부터 받음
    // SW1이 1초 이상 눌리면 SW1_debounce=1
    SW1_prev = SW1_curr;
    SW1_curr = (PORT02_IN & (1<<P0)) == 0;
    SW2_prev = SW2_curr;
    SW2_curr = (PORT02_IN & (1<<P1)) == 0;

    // SW1, SW2 software debounce for 1 second
    if( SW1_curr != SW1_prev )
        SW1_cnt = 0;
    else if( SW1_cnt < 5 )
        SW1_cnt ++;
    else
        SW1_debounce = SW1_curr;


    if( SW2_curr != SW2_prev )
        SW2_cnt = 0;
    else if( SW2_cnt < 5 )
        SW2_cnt ++;
    else
        SW2_debounce = SW2_curr;

    irq_timer = 1;
}
