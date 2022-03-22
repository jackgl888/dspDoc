
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2837xD_Ipc_drivers.h"

void GPIO_Setup(void);
void SEG_DATA(unsigned char d);

#define   SBIT3     GpioDataRegs.GPBDAT.bit.GPIO58
#define   SBIT2     GpioDataRegs.GPBDAT.bit.GPIO59
#define   SBIT1     GpioDataRegs.GPBDAT.bit.GPIO60
#define   SBIT0     GpioDataRegs.GPBDAT.bit.GPIO61

unsigned char const table[]={//������0~f����ܱ���
0xfc,0x60,0xda,0xf2,     //0~3
0x66,0xb6,0xbe,0xe0,    //4~7
0xfe,0xf6               //8~9
};

void main(void)
{
// Step 1. ��ʼ��ϵͳ�����໷,���Ź�������ʱ��
   InitSysCtrl();

#ifdef _STANDALONE
#ifdef _FLASH
// ������������������CPU2Ӧ�ó���ʼִ����д
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
// ������������������CPU2Ӧ�ó���ʼ����
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif
#endif


#ifdef _FLASH
   InitFlash();
#endif

// Step 2. ��ʼ��GPIO:
    InitGpio();
    GPIO_Setup();
// Step 3. ��������жϣ���ʼ���ж���������ֹCPU�ж�
    DINT;

// ��ʼ��PIE���ƼĴ�����Ĭ��״̬��Ĭ��״̬����PIE�жϱ���ֹ�����б�־λ�������
    InitPieCtrl();

//��ֹ����CPU�жϣ����CPU�ж���ر�־λ��
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
    InitPieVectTable();

// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    SBIT0=1;DELAY_US(1);
    SBIT1=1;DELAY_US(1);
    SBIT2=1;DELAY_US(1);
    SBIT3=1;DELAY_US(1);

    SEG_DATA(table[2]);
    while(1)
    {
        SEG_DATA(table[0]);
        SBIT0=0;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT1=1;DELAY_US(1);
        SBIT3=1;
        DELAY_US(100);
        SBIT0=1;
        SEG_DATA(0);

        SEG_DATA(table[1]);
        SBIT1=0;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT3=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT1=1;
        SEG_DATA(0);

        SEG_DATA(table[2]);
        SBIT2=0;DELAY_US(1);
        SBIT1=1; DELAY_US(1);
        SBIT3=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT2=1;
        SEG_DATA(0);

        SEG_DATA(table[3]);
        SBIT3=0;DELAY_US(1);
        SBIT1=1;DELAY_US(1);
        SBIT2=1;DELAY_US(1);
        SBIT0=1;
        DELAY_US(100);
        SBIT3=1;
        SEG_DATA(0);
    }
}

void GPIO_Setup()
{
    GPIO_SetupPinMux(58, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(58, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(59, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(59, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(60, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(60, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(61, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(61, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(77, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(77, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(78, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(78, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(79, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(79, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(80, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(80, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(81, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(81, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(82, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(82, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(83, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(83, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(85, GPIO_OUTPUT, GPIO_PUSHPULL);
}


void SEG_DATA(unsigned char d)
{
    Uint32 dat1 = 0,dat2 = 0;
     dat1 = d;
     dat1 <<= 13; //����13λ�� C��GPIO�Ǵ�GPIO64��ʼ��
     dat2 = d;
     dat2 <<= 13;
     dat2 &= 0x100000;
     dat2 <<= 1;
     dat1 |= dat2;
     GpioDataRegs.GPCDAT.all = dat1;
}
