#include "F28x_Project.h"
#include "F2837xD_Ipc_drivers.h"

void GPIO_Setup(void);
//�����̣���LED�ø��õͽ��к궨�壻
#define LED1_ON  GpioDataRegs.GPASET.bit.GPIO0 = 1      //GPIO0�ø�
#define LED1_OFF GpioDataRegs.GPACLEAR.bit.GPIO0 = 1    //GPIO�õ�
#define LED2_ON  GpioDataRegs.GPASET.bit.GPIO1 = 1
#define LED2_OFF GpioDataRegs.GPACLEAR.bit.GPIO1 = 1
#define LED3_ON  GpioDataRegs.GPASET.bit.GPIO2 = 1
#define LED3_OFF GpioDataRegs.GPACLEAR.bit.GPIO2 = 1
#define LED4_ON  GpioDataRegs.GPASET.bit.GPIO3 = 1
#define LED4_OFF GpioDataRegs.GPACLEAR.bit.GPIO3 = 1
#define LED5_ON  GpioDataRegs.GPASET.bit.GPIO4 = 1
#define LED5_OFF GpioDataRegs.GPACLEAR.bit.GPIO4 = 1
#define LED6_ON  GpioDataRegs.GPASET.bit.GPIO5 = 1
#define LED6_OFF GpioDataRegs.GPACLEAR.bit.GPIO5 = 1


void main(void)
{
// Step 1.  ��ʼ��ϵͳ�����໷,���Ź�������ʱ��
   InitSysCtrl();

#ifdef _STANDALONE
#ifdef _FLASH
// CPU2���г���
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
// CPU2���г���
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif
#endif

// FALSH��д����
#ifdef _FLASH
   InitFlash();
#endif

// Step 2. ��ʼ��GPIO:
    InitGpio();
    GPIO_Setup();
// Step 3. ��������жϣ���ʼ���ж���������ֹCPU�жϡ�
    DINT;

// ��ʼ��PIE���ƼĴ�����Ĭ��״̬��Ĭ��״̬����PIE�жϱ���ֹ�����б�־λ�������
    InitPieCtrl();

// ��ֹ����CPU�жϣ����CPU�ж���ر�־λ��
    IER = 0x0000;
    IFR = 0x0000;

// ��ʼ���ж��������ж���������ǲ���ָ��ָ���жϷ��������ڵ�ַ
    InitPieVectTable();

// Enable global Interrupts and higher priority real-time debug events:
    EINT;  //ʹ��ȫ���ж�
    ERTM;  // ʹ��ȫ���ж�ʵʱ����

// Step 6. ѭ��
    for(;;)
    {
        //
        // ��LED
        //
    	LED1_ON;
    	LED2_ON;
    	LED3_ON;
    	LED4_ON;
    	LED5_ON;
    	LED6_ON;
        //
        // �ӳ�0.5S
        //
    	DELAY_US(1000 * 500);

        //
        // ��LED
        //
        LED1_OFF;
        LED2_OFF;
        LED3_OFF;
        LED4_OFF;
        LED5_OFF;
        LED6_OFF;
        //
        // �ӳ�0.5s.
        //
    	DELAY_US(1000 * 500);
    }
}

void GPIO_Setup() //����GPIO�Ĺ���
{
    GPIO_SetupPinMux(0, GPIO_MUX_CPU1, 0);//������0���ã�ѡ��CPU1Ϊ�ںˣ�ѡ�����蹦��0
    GPIO_SetupPinOptions(0, GPIO_OUTPUT, GPIO_PUSHPULL);//GPIO0����Ϊ���������
    GPIO_SetupPinMux(1, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(1, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(2, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(3, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(4, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(5, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(5, GPIO_OUTPUT, GPIO_PUSHPULL);
}
