#include "Lock_Unlock.h"
#include "my_lib.h"
#include "Status.h"
#include "Led.h"

int stack[5]={0,};

unsigned int Unlock(){
    stack_cnt = 0;
    potential_meter_duty = 0;
    irq_timer = 0;
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
    while(1)
        {
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
                PORT10_OMR = (1<<PCL2);                             // LED BLUE  off
                PORT10_OMR = (1<<PC1);
                GTM_TOM0_CH2_SR0 = 12500 - 1;
                GTM_TOM0_CH2_SR1 = potential_meter_duty;
            }


            // stack_cnt==8이면 stack과 비교
            if(stack_cnt==8){
                if(check_pwd(stack)){
                    // 만약 일치하면 BLUE LED on상태, while문 break
                    PORT10_OMR = (1<<PCL2);                         // LED BLUE  off
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PS2)   ;                       // LED BLUE  on
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PCL2);                         // LED BLUE  off
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PS2)   ;                       // LED BLUE  on
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PCL2);                         // LED BLUE  off
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    return 1;
                }
                else{
                    //비교한게 틀렸으면 두번 RED blink, stack_cnt초기화, stack초기화
                    stack_cnt=0;
                    for(unsigned int i=0;i<5;i++){
                        stack[i]=0;
                    PORT10_OMR = (1<<PS1)   ;                       // LED RED  on
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PCL1);                         // LED RED  off
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PS1)   ;                       // LED RED  on
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    PORT10_OMR = (1<<PCL1);                         // LED RED  off
                    for(unsigned int delay=0;delay<DELAY/6;delay++);
                    }
                    return 0;
                }
            }
        }
}

int check_pwd(int *stack){
    // stack의 크기는 4로 고정
    int value = 1;
    volatile unsigned int PWD[4]={1,1,1,1};
    for(volatile int i=0;i<4;i++){
        if(stack[i]!=PWD[i]){
            value=0;                //하나라도 틀리면 value=0이 되고 break
            break;                  //다 맞으면 value는 1로 유지
        }
    }
    return value;
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
