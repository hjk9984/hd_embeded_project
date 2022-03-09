#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

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

void init_ultrasonic(void);

#endif /* ULTRASONIC_H_ */
