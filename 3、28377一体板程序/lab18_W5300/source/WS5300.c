
#include "F28x_Project.h"
#include "main.h"
#include "xscale.h"
#include "socket.h"
//#define W5300RCTRL      GpioDataRegs.GPCSET.bit.GPIO94  // Íø¿ÚÐ¾Æ¬¸´Î»¼Ä´æÆ÷
uchar ERROR_HTML_PAGE[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 78\r\n\r\n<HTML>\r\n<BODY>\r\nSorry, the page you requested was not found.\r\n</BODY>\r\n</HTML>\r\n\0"; 
//LOADER_STATUS	status;           // for serial port
uint8 dc[2048];
uint8 i=0;
Uint16  ErrCount = 0;
extern void setup_emif1_pinmux_async_16bit(Uint16 cpu_sel);
/* Definition of example code */
void     loopback_tcps(SOCKET s, uint16 port, uint8* buf,uint16 mode);
void     loopback_tcpc(SOCKET s, uint8* addr, uint16 port, uint8* buf,uint16 mode);
void     loopback_udp(SOCKET s, uint16 port, uint8* buf, uint16 mode);
void resetw5300(void)
{
	GpioDataRegs.GPDSET.bit.GPIO99=1;//GPIO_WritePin(94, 1);
	DELAY_US(1000);
	GpioDataRegs.GPDCLEAR.bit.GPIO99=1;//GPIO_WritePin(94, 0);
	DELAY_US(20000);
	GpioDataRegs.GPDSET.bit.GPIO99=1;//GPIO_WritePin(94, 1);
	DELAY_US(1000);  
}


void initMCU(void)
{
	InitSysCtrl();
	//InitXintf();

	InitGpio();

    GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(30, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPASET.bit.GPIO30=1;

    GPIO_SetupPinMux(99, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(99, GPIO_OUTPUT, GPIO_PUSHPULL);
	DINT;
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();


	  InitEmif1();

	 EDIS;


	GpioDataRegs.GPASET.bit.GPIO30=1;
	//Configure GPIO pins for EMIF1
	EINT; 
	ERTM;   
} 

/**
 * It executes example program such as loopback_tcps(), loopback_tcpc(), and loopback_udp().
 */ 
void main()
{
   uint8 tx_mem_conf[8] = {8,8,8,8,8,8,8,8};          // for setting TMSR regsiter
   uint8 rx_mem_conf[8] = {8,8,8,8,8,8,8,8};          // for setting RMSR regsiter
   
   uint8 * data_buf;         // buffer for loopack data
   
   uint8 ip[4] = {192,168,1,11};                   // for setting SIP register
   uint8 gw[4] = {192,168,1,1};                     // for setting GAR register
   uint8 sn[4] = {255,255,255,0};                     // for setting SUBR register
   uint8 mac[6] = {0x00,0x08,0xDC,0x00,111,200};      // for setting SHAR register
   
   uint8 serverip[4] = {192,168,0,18};              // "TCP SERVER" IP address for loopback_tcpc()
   
 //  status.terminalSpeed = SERIAL_SPEED;
//   status.downloadSpeed = SERIAL_DOWNLOAD_SPEED;
   
   data_buf = dc;                       
   initMCU();
   GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 0);
       GPIO_SetupPinOptions(30, GPIO_OUTPUT, GPIO_PUSHPULL);
       GpioDataRegs.GPASET.bit.GPIO30=1;
   resetw5300();
	/* initiate W5300 */
   iinchip_init();
   /* allocate internal TX/RX Memory of W5300 */
   if(!sysinit(tx_mem_conf,rx_mem_conf))           
   {
      ////printf("MEMORY CONFIG ERR.\r\n");
      while(1);
   }

   //setMR(getMR()|MR_FS);                            // If Little-endian, set MR_FS.

   setSHAR(mac);                                      // set source hardware address
   
   #ifdef __DEF_IINCHIP_PPP__
      if(pppinit((uint8*)"test01", 6, (uint8*)"pppoe1000", 9)!=1)
      {
         //printf("PPPoE fail.\r\n");
         while(1);
      }
      close(0);
   #else
      /* configure network information */
      setGAR(gw);                                     // set gateway IP address
      setSUBR(sn);                                    // set subnet mask address
      setSIPR(ip);                                    // set source IP address
   #endif
   
   /* verify network information */
   getSHAR(mac);                                      // get source hardware address 
   getGAR(gw);                                        // get gateway IP address      
   getSUBR(sn);                                       // get subnet mask address     
   getSIPR(ip);                                       // get source IP address       
 #define TCP_PORT 1026  
   while(1)
   {
      loopback_tcps(0,TCP_PORT,data_buf,0);
      loopback_tcps(1,(TCP_PORT+1),data_buf,0);
      loopback_tcps(2,(TCP_PORT+2),data_buf,0);
      loopback_tcps(3,(TCP_PORT+3),data_buf,0);
      loopback_tcps(4,(TCP_PORT+4),data_buf,0);
      loopback_tcps(5,(TCP_PORT+5),data_buf,0);
	//  loopback_tcps(6,(TCP_PORT+6),data_buf,0);
	  //loopback_tcps(7,(TCP_PORT+7),data_buf,0);
      loopback_tcpc(7,serverip, 3000, data_buf,0);
      loopback_udp(6,3000,data_buf,0);
   }

   #ifdef __DEF_IINCHIP_PPP__
   {
      uint8 ppp_mac[6];
      getPDHAR(ppp_mac);
      pppterm(ppp_mac, getPSIDR());
   }
   #endif
//   while(1);
}


/**
 * "TCP SERVER" loopback program.
 */ 
void     loopback_tcps(SOCKET s, uint16 port, uint8* buf, uint16 mode)
{
   uint32 len;
   
   switch(getSn_SSR(s))                // check SOCKET status
   {                                   // ------------
      case SOCK_ESTABLISHED:           // ESTABLISHED?
         if(getSn_IR(s) & Sn_IR_CON)   // check Sn_IR_CON bit
         {
            ////printf("%d : Connect OK\r\n",s);
            setSn_IR(s,Sn_IR_CON);     // clear Sn_IR_CON
         }
         if((len=getSn_RX_RSR(s)) > 0) // check the size of received data
         {
            len = recv(s,buf,len);     // recv
		//	memcpy(buf, ERROR_HTML_PAGE, sizeof(ERROR_HTML_PAGE));
	/*	len=0;
		i=0;
		    while(ERROR_HTML_PAGE[len] != '\0'){
			dc[len]=0;
               dc[len] = ERROR_HTML_PAGE[i];
			   dc[len] <<= 8;
			  i++;
			   if(ERROR_HTML_PAGE[i] == '\0') break;
			   dc[len] |= ERROR_HTML_PAGE[i];
			   len++;i++;
			}
			send(s, buf, 73); */
           if(len !=send(s,buf,len))  //send
            {
               ////printf("%d : Send Fail.len=%d\r\n",s,len);
            }
         }
         break;
                                       // ---------------
   case SOCK_CLOSE_WAIT:               // PASSIVE CLOSED
         disconnect(s);                // disconnect 
         break;
                                       // --------------
   case SOCK_CLOSED:                   // CLOSED
      close(s);                        // close the SOCKET
      socket(s,Sn_MR_TCP,port,mode);   // open the SOCKET  
      break;
                                       // ------------------------------
   case SOCK_INIT:                     // The SOCKET opened with TCP mode
      listen(s);                       // listen to any connection request from "TCP CLIENT"
      ////printf("%d : LOOPBACK_TCPS(%d) Started.\r\n",s,port);
      break;
   default:
      break;
   }
}

/**
 * "TCP CLIENT" loopback program.
 */ 
void     loopback_tcpc(SOCKET s, uint8* addr, uint16 port, uint8* buf, uint16 mode)
{
   uint32 len;
   static uint16 any_port = 1000;
   
   switch(getSn_SSR(s))                   // check SOCKET status
   {                                      // ------------
      case SOCK_ESTABLISHED:              // ESTABLISHED?
         if(getSn_IR(s) & Sn_IR_CON)      // check Sn_IR_CON bit
         {
            ////printf("%d : Connect OK\r\n",s);
            setSn_IR(s,Sn_IR_CON);        // clear Sn_IR_CON
         }
         if((len=getSn_RX_RSR(s)) > 0)    // check the size of received data
         {
            len = recv(s,buf,len);        // recv
            if(len !=send(s,buf,len))     // send
            {
               //printf("%d : Send Fail.len=%d\r\n",s,len);
            }
         }
         break;
                                          // ---------------
   case SOCK_CLOSE_WAIT:                  // PASSIVE CLOSED
         disconnect(s);                   // disconnect 
         break;
                                          // --------------
   case SOCK_CLOSED:                      // CLOSED
      close(s);                           // close the SOCKET
      socket(s,Sn_MR_TCP,any_port++,mode);// open the SOCKET with TCP mode and any source port number
      break;
                                          // ------------------------------
   case SOCK_INIT:                        // The SOCKET opened with TCP mode
      connect(s, addr, port);             // Try to connect to "TCP SERVER"
      break;
   default:
      break;
   }
}

/**
 * UDP loopback program.
 */ 
void     loopback_udp(SOCKET s, uint16 port, uint8* buf, uint16 mode)
{
   uint32 len;
   uint8 destip[4];
   uint16 destport;
   
   switch(getSn_SSR(s))
   {
                                                         // -------------------------------
      case SOCK_UDP:                                     // 
         if((len=getSn_RX_RSR(s)) > 0)                   // check the size of received data
         {
            len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
            if(len !=sendto(s,buf,len,destip,destport))  // send the data to the destination
            {
               //printf("%d : Sendto Fail.len=%d,",s,len);
               //printf("%d.%d.%d.%d(%d)\r\n",destip[0],destip[1],destip[2],destip[3],destport);
            }
         }
         break;
                                                         // -----------------
      case SOCK_CLOSED:                                  // CLOSED
         close(s);                                       // close the SOCKET
         socket(s,Sn_MR_UDP,port,mode);                  // open the SOCKET with UDP mode
         break;
      default:
         break;
   }
}
