#include "SD.h"
#include "F28x_Project.h"
#include "sdio_sd.h"
#include "ff.h"
#include <stdio.h>

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */   
FRESULT res=FR_OK;
UINT br;
#define SPI_BRR        ((200E6 / 4) / 500E3) - 1
char path[512]="0:";
Uint8 textFileBuffer[] = "感谢您使用研旭28377开发版 ！^_^ \r\n";
void InitSpibGpio()
{

   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;    // Enable pull-up on GPIO5 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;    // Enable pull-up on GPIO3 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;   // Enable pull-up on GPIO19 (SPISTEA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;  // Asynch input GPIO5 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; // Asynch input GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;  // Asynch input GPIO3 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; // Asynch input GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO19 (SPISTEA)

/* Configure SPI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 1; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 2;
//  GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;  // Configure GPIO5 as SPISIMOA
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 2;
//  GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;  // Configure GPIO3 as SPISOMIA
    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 1; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 2;
//    GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 1; // Configure GPIO19 as SPISTEA
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;
    GpioDataRegs.GPBSET.bit.GPIO59 = 1;

    EDIS;
}
void InitSpiGpio()
{
   InitSpibGpio();
}
void InitSpi(void)
{
    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpibRegs.SPICCR.bit.SPISWRESET = 0;
    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpibRegs.SPICCR.bit.SPICHAR = (16-1);
    SpibRegs.SPICCR.bit.SPILBK = 1;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpibRegs.SPICTL.bit.TALK = 1;
    SpibRegs.SPICTL.bit.CLK_PHASE = 0;
    SpibRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpibRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpibRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpibRegs.SPICCR.bit.SPISWRESET = 1;
}
void spi_fifo_init()
{
    //
    // Initialize SPI FIFO registers
    //
    SpibRegs.SPIFFTX.all = 0xE040;
    SpibRegs.SPIFFRX.all = 0x2044;
    SpibRegs.SPIFFCT.all = 0x0;

    //
    // Initialize core SPI registers
    //
    InitSpi();
}
void spi_xmit(Uint16 a)
{
    SpibRegs.SPITXBUF = a;
}
#define sd_spi
void main(void)
{

//	Uint16 i, testdata = 0x0000;
   FRESULT h;
   char m[]="0:/fatfs.txt";
   // Step 1. Initialize System Control:
   // PLL, WatchDog, enable Peripheral Clocks
   // This example function is found in the F2837xD_SysCtrl.c file.
   //
      InitSysCtrl();

   //
   // Step 2. Initialize GPIO:
   // This example function is found in the F2837xD_Gpio.c file and
   // illustrates how to set the GPIO to it's default state.
   // Setup only the GP I/O only for SPI-A functionality
   // This function is found in F2837xD_Spi.c
   //
      InitSpiGpio();

   //
   // Step 3. Clear all interrupts:
   //
      DINT;

   //
   // Initialize PIE control registers to their default state.
   // The default state is all PIE __interrupts disabled and flags
   // are cleared.
   // This function is found in the F2837xD_PieCtrl.c file.
   //
      InitPieCtrl();

   //
   // Disable CPU __interrupts and clear all CPU __interrupt flags:
   //
      IER = 0x0000;
      IFR = 0x0000;

   //
   // Initialize the PIE vector table with pointers to the shell Interrupt
   // Service Routines (ISR).
   // This will populate the entire table, even if the __interrupt
   // is not used in this example.  This is useful for debug purposes.
   // The shell ISR routines are found in F2837xD_DefaultIsr.c.
   // This function is found in F2837xD_PieVect.c.
   //
      InitPieVectTable();

   //
   // Step 4. Initialize the Device Peripherals:
//      spi_fifo_init();
      spi_initialization();    // Initialize the SPI FIFO
// 	Step 5. User specific code:	

	//Initialize test data for known data to transmit to card
/*	for(i=0;i<1024;i++)						 
	{										
		write_buffer[i] = testdata;
		testdata += 0x0100;			//Card accepts 8 bit data so upper 8 bits are used
	}

   	sd_card_insertion();			//Check for card insertion
	sd_initialization();			//Initialize card

	sd_read_register(SEND_CSD);		//Read CSD register
	sd_read_register(READ_OCR);		//Read OCR register
	sd_read_register(SEND_CID);		//Read CID register
*/
//	disk_initialize(0);
#ifdef sd_spi
    res = f_unlink(m);//删除SD卡内同名文件
	h=f_mount(0,&fs);  
	if(h!=FR_OK)  asm(" ESTOP0"); //文件注册失败，程序暂停

	res = f_open( &fsrc , m , FA_CREATE_ALWAYS | FA_WRITE);//FA_CREATE_NEW | FA_WRITE);		

    if ( res == FR_OK )
    { 
      /* Write buffer to file */
      res = f_write(&fsrc, textFileBuffer, sizeof(textFileBuffer), &br);     
 
    
      /*close file */
      f_close(&fsrc); //如果此句屏蔽则文件内容不会被写入SD卡     
    }
//	res = f_unlink(m);
//	res=f_read(&fsrc,read,sizeof(textFileBuffer), &br);
	
    asm(" ESTOP0");				//Emulation stop
   	for(;;){};					//Loop forever
#else
   	Uint16 sdata;
    sdata = 0x0000;
    Uint16 rdata;
    for(;;)
    {
         //
         // Transmit data
         //
         spi_xmit(sdata);

         //
         // Wait until data is received
         //
         while(SpibRegs.SPIFFRX.bit.RXFFST !=1) { }

         //
         // Check against sent data
         //
         rdata = SpibRegs.SPIRXBUF;
         if(rdata != sdata)
         {
             asm("     ESTOP0");     // Test failed!! Stop!
             for (;;);
         }
         sdata++;
    }
#endif
} 	







//===========================================================================
// No more.
//===========================================================================

