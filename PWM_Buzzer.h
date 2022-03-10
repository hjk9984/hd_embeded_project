/* Address of Registers */
// SCU Registers
#define SCU_BASE        (0xF0036000)
#define SCU_WDT_CPU0CON0 (*(volatile unsigned int*)(SCU_BASE + 0x100))

#define LCK             1
#define ENDINIT         0

// GTM Registers
// GTM - CMU
#define GTM_BASE                    (0xF0100000)
#define GTM_CMU_CLK_EN              (*(volatile unsigned int*)(GTM_BASE + 0x00300))
#define GTM_CMU_FXCLK_CTRL          (*(volatile unsigned int*)(GTM_BASE + 0x00344))

#define EN_FXCLK                    22
#define FXCLK_SEL                   0

// GTM - TOM0
#define GTM_TOM0_TGC1_GLB_CTRL      (*(volatile unsigned int*)(GTM_BASE + 0x08230))
#define GTM_TOM0_TGC1_ENDIS_CTRL    (*(volatile unsigned int*)(GTM_BASE + 0x08270))
#define GTM_TOM0_TGC1_OUTEN_CTRL    (*(volatile unsigned int*)(GTM_BASE + 0x08278))
#define GTM_TOM0_TGC1_FUPD_CTRL     (*(volatile unsigned int*)(GTM_BASE + 0x08238))
#define GTM_TOM0_CH11_CTRL          (*(volatile unsigned int*)(GTM_BASE + 0x082C0))
#define GTM_TOM0_CH11_SR0           (*(volatile unsigned int*)(GTM_BASE + 0x082C4))
#define GTM_TOM0_CH11_SR1           (*(volatile unsigned int*)(GTM_BASE + 0x082C8))

#define UPEN_CTRL3                  22
#define HOST_TRIG                   0
#define ENDIS_CTRL3                 6
#define OUTEN_CTRL3                 6
#define RSTCN0_CH3                  22
#define FUPD_CTRL3                  6
#define CLK_SRC_SR                  12
#define SL                          11

// GTM
#define GTM_CLC                     (*(volatile unsigned int*)(GTM_BASE + 0x9FD00))
#define GTM_TOUTSEL0                (*(volatile unsigned int*)(GTM_BASE + 0x9FD30))

#define DISS                        1
#define DISR                        0
#define SEL3                        6

// PORT02 Registers
#define PORT02_BASE     (0xF003A200)
#define PORT02_IOCR0    (*(volatile unsigned int*)(PORT02_BASE + 0x10))
#define PORT02_PDR0     (*(volatile unsigned int*)(PORT02_BASE + 0x40))

#define PC3             27

/* Function Prototype */
void init_Buzzer(void);
void init_GTM_TOM0_PWM(void);
void set_pd(int pd);
void note(int,int);

#define PWM_FREQ        6250000
