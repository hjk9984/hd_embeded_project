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
int stack[5];
volatile unsigned int stack_cnt;

unsigned int Unlock(void);
int check_pwd(int *stack);
