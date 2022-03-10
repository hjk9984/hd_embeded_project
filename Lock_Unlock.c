#include "Lock_Unlock.h"
#include "my_lib.h"
#include "Status.h"
#include "Led.h"
#include "PWM_Buzzer.h"

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

            //buzzer on when switch is pushed
            if(SW1_debounce_prev == 1 && SW1_debounce == 0)
                note(0, 16);

            if(SW2_debounce_prev == 1 && SW2_debounce == 0)
                note(1, 16);

//            if(SW1_debounce_prev == 1 && SW1_debounce == 0){
//                            note(0, 16);
//                    }
            SW_state_prev = SW_state_curr;

            SW_state_curr = (SW2_debounce<<1) | (SW1_debounce<<0);          // [sw2_debounce,sw1_debounce]

            if(SW_state_curr!=SW_state_prev)
                SW_state_cnt=0;
            else if(SW_state_cnt<5){
                SW_state_cnt++;
            }
            else{
                SW_state_debounce=SW_state_curr;
            }

            if( SW_state_debounce != 0 )                            // ���� ������ debounce�� ��ȭ�� �����
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
                else                                                // �Ѵ� ������ error�� ����, stack�ʱ�ȭ, default��...
                {
                    stack_cnt=0;
                    for(volatile int i =0;i<5;i++)
                        stack[i]=0;

                    // Ʋ���� �ι� RED blink
                    PORT10_OMR = (1<<PCL1);                         // LED RED  off
                    PORT10_OMR = (1<<PS1)  ;                        // LED RED  on
                    PORT10_OMR = (1<<PS1) ;                         // LED RED  on
                }

            }
            else
            // �ƹ��͵� ������ ���� ����
            // RED LED�� �� ����
            {
                PORT10_OMR = (1<<PCL2);                             // LED BLUE  off
                PORT10_OMR = (1<<PC1);
                GTM_TOM0_CH2_SR0 = 12500 - 1;
                GTM_TOM0_CH2_SR1 = potential_meter_duty;
            }


            // stack_cnt==8�̸� stack�� ��
            if(stack_cnt==8){
                if(check_pwd(stack)){
                    // ���� ��ġ�ϸ� BLUE LED on����, while�� break
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
                    //���Ѱ� Ʋ������ �ι� RED blink, stack_cnt�ʱ�ȭ, stack�ʱ�ȭ
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
    // stack�� ũ��� 4�� ����
    int value = 1;
    volatile unsigned int PWD[4]={1,1,1,1};
    for(volatile int i=0;i<4;i++){
        if(stack[i]!=PWD[i]){
            value=0;                //�ϳ��� Ʋ���� value=0�� �ǰ� break
            break;                  //�� ������ value�� 1�� ����
        }
    }
    return value;
}

// 100ms timer
__interrupt( 0x0F ) __vector_table( 0 )
void CCU61_T12_ISR(void)
{
    // ���⼱ interrupt�� SW1,2�κ��� ����
    // SW1�� 1�� �̻� ������ SW1_debounce=1
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
