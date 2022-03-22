
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
void ConfigureADC(void);
void ConfigureEPWM(void);
void SetupADCEpwm(Uint16 channel);
interrupt void adca1_isr(void);

//buffer for storing conversion results
#define RESULTS_BUFFER_SIZE 512
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex;
volatile Uint16 bufferFull;

void main(void)
{
// Step 1. ��ʼ��ϵͳ�����໷,���Ź�������ʱ��

    InitSysCtrl();

// Step 2.��ʼ��GPIO
    InitGpio();
    
// ��������жϣ���ʼ���ж���������ֹCPU�ж�
    DINT;

//��ʼ��PIE���ƼĴ�����Ĭ��״̬��Ĭ��״̬����PIE�жϱ���ֹ�����б�־λ�������
    InitPieCtrl();

// ��ֹ����CPU�жϣ����CPU�ж���ر�־λ
    IER = 0x0000;
    IFR = 0x0000;

//��ʼ���ж��������ж���������ǲ���ָ��ָ���жϷ��������ڵ�ַ
    InitPieVectTable();

    EALLOW;//д����
    PieVectTable.ADCA1_INT = &adca1_isr;
    EDIS;

//��ʼ��ADC
    ConfigureADC();

//��ʼ�� ePWM
    ConfigureEPWM();

//����ADCΪPWM����
    SetupADCEpwm(0);

//ʹ��CPU�ж�
    IER |= M_INT1; //ʹ�ܵ�һ���ж�
    EINT;  // ʹ��ȫ���ж�
    ERTM;  // ʹ�ܵ���

//Initialize results buffer
    for(resultsIndex = 0; resultsIndex < RESULTS_BUFFER_SIZE; resultsIndex++)
    {
    	AdcaResults[resultsIndex] = 0;
    }
    resultsIndex = 0;
    bufferFull = 0;

//ʹ��PIE�ж�
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;


    do
    {
    	//����ePWM
        EPwm1Regs.ETSEL.bit.SOCAEN = 1;  //ʹ�� SOCA
        EPwm1Regs.TBCTL.bit.CTRMODE = 0; //unfreeze, and enter up count mode

    	//wait while ePWM causes ADC conversions, which then cause interrupts,
    	//which fill the results buffer, eventually setting the bufferFull
    	//flag
    	while(!bufferFull);
    	bufferFull = 0; //clear the buffer full flag

    	//stop ePWM
        EPwm1Regs.ETSEL.bit.SOCAEN = 0;  //disable SOCA
        EPwm1Regs.TBCTL.bit.CTRMODE = 3; //freeze counter

    }while(1);
}

void ConfigureADC(void)
{
	EALLOW;

	//write configurations
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

	//Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	//power up the ADC
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}

void ConfigureEPWM(void)
{
	EALLOW;
	// Assumes ePWM clock is already enabled
	EPwm1Regs.ETSEL.bit.SOCAEN	= 0;	        // Disable SOC on A group
	EPwm1Regs.ETSEL.bit.SOCASEL	= 4;	        // Select SOC on up-count
	EPwm1Regs.ETPS.bit.SOCAPRD = 1;		        // Generate pulse on 1st event
	EPwm1Regs.CMPA.bit.CMPA = 0x0800;          // Set compare A value to 2048 counts
	EPwm1Regs.TBPRD = 0x1000;			        // Set period to 4096 counts
	EPwm1Regs.TBCTL.bit.CTRMODE = 3;            // freeze counter
	EDIS;
}

void SetupADCEpwm(Uint16 channel)
{
	Uint16 acqps;

	//determine minimum acquisition window (in SYSCLKS) based on resolution
	if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
		acqps = 14; //75ns
	}
	else { //resolution is 16-bit
		acqps = 63; //320ns
	}

	//Select the channels to convert and end of conversion flag
	EALLOW;
	AdcaRegs.ADCSOC0CTL.bit.CHSEL = channel;  //SOC0 will convert pin A0
	AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
}

interrupt void adca1_isr(void)
{
	AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT0;
	if(RESULTS_BUFFER_SIZE <= resultsIndex)
	{
		resultsIndex = 0;
		bufferFull = 1;
	}

	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
