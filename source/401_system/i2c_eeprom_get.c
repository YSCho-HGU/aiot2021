//// File Name    : i2ceeprom.c                                          
//// Description  : i2c eeprom(24C02) test application for raspberry pi3 
////                respberry Pi3 보드에 AT24C02을 연결하고 테스트한다. 
//// Reference    : ATMEL 24C02 datasheet          
//// Last Update  : 2017.02.16     

//// Include Header Files //////////////////////////////////////////////////////
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c-dev.h>

//// Declare Global Constants //////////////////////////////////////////////////
#define SLAVE_EEPROM_ADDR  0x50  // Slave address of 24C02 EEPROM

//// AT24C02 command ///////////////////////////////////////////////////////////
#define BYTEWRITE          1  // Byte write command
#define PAGEWRITE          2  // Page write command
#define RNDREAD            3  // Random read command
#define SEQREAD            4  // Sequential read command

//// Define Data Types /////////////////////////////////////////////////////////
typedef unsigned char       byte;   // 8-bit  Data, initial b
typedef unsigned short int  ushort; // 16-bit Unsigned Short Integer initial us
typedef unsigned int        uint;   // 32-bit Unsigned Integer,initial u
typedef unsigned long int   ulong;  // 64-bit Unsigned Long Integer , initial ul

//// Declare Global Variables //////////////////////////////////////////////////
struct i2c_rdwr_ioctl_data gmsg_rdwr;
struct i2c_msg             gi2cmsg;

//// Declare Functions /////////////////////////////////////////////////////////
int i2cTransfer(uint uFd, byte bCmd, byte bAddr, byte *pbData, uint uLen);

//// Define Functions //////////////////////////////////////////////////////////

//// Function Name : main
//// Operation     : main 함수
uint main(void)
{
   int i, j;
  uint uFd; 
  byte brndNum;
  byte breadData;
  byte abrData[8];
  byte abTest[8];
 
    //// open i2c eeprom ///////////////////////////////////////////////////////  
    if ((uFd = open ("/dev/i2c-1", O_RDWR) ) < 0)
    {
        printf("can't open i2c device\n");
        return -1 ;      
    }  

    // EEPROM(24C02)전체 영역에 대한 write/read test를 한다.
    printf("==== random write/read test start ====\n");
    for (i =0 ; i < 256 ; i++)
    {  
       printf("address : %3d ", i);     
        brndNum = rand() % 256;   // random number 생성
        // 생성한 random number 1byte write 
        i2cTransfer(uFd, BYTEWRITE, i, &brndNum, 1);
        usleep(5000);  // delay 5ms
        printf("write data : 0x%.2X ", brndNum); 
       
        // 방금 write 한 data를 read
        i2cTransfer(uFd, RNDREAD, i, &breadData, 1);
       usleep(5000);  // delay 5ms
       printf("read data : 0x%.2X\n ", breadData);
      
       // write 한 data와 read data를 비교한다.
       // 만일, 비교결과가 다르면 오류 메세지를 출력하다.
        if (brndNum != breadData) 
        {     
           printf("Result  : TEST Fail\n\n");
            return -1;
        }    
    }
    printf("==== random write/read test ok ====\n\n");

    // EEPROM(24C02)전체 영역에 대한 page write/sequential read test를 한다.
    printf("==== page write/sequential read test start ====\n");
    for (i =0 ; i < 8 ; i++)
        abTest[i] = rand() % 256;
   
    for (i = 0 ; i < 32 ; i++)
    {
        i2cTransfer(uFd, PAGEWRITE, (i * 8), abTest, 8);
        usleep(5000);  // delay 5ms
       
        i2cTransfer(uFd, SEQREAD, (i * 8), abrData, 8);
        usleep(5000);  // delay 5ms
       
        // write한 data와 read한 데이터를 비교한다.
        for (j = 0 ; j < 8 ; j++)
        {
            if (abTest[j] != abrData[j])
            {
                printf("page write/sequential read test fail\n");
                return -1;
            }
        }
    }
    printf("==== page write/sequential read test ok ====\n\n");

   close(uFd);
  
    return 0; 
}

//// Function Name : i2cTransfer
//// Operation     : I2C Device(24C02)에 data를 read/write한다.
////                 (input) uFd
////                 (input) bCmd
////                 (input) bAddr , word address
////                 (in/out) *pbData, read/write data buffer
////                 (input) uLen
int i2cTransfer(uint uFd, byte bCmd, byte bAddr, byte *pbData, uint uLen)
{
  byte bi;
  int i;
  uint ui;
  byte bRxbuff[256];
  byte bTxbuff[9];
  
   gmsg_rdwr.msgs = &gi2cmsg;
   gmsg_rdwr.nmsgs = 1;
   gi2cmsg.addr  = SLAVE_EEPROM_ADDR;
    
    if (bCmd == BYTEWRITE)
    {              
        bTxbuff[0] = bAddr; // set word address
        bTxbuff[1] = *pbData; // write data 
       
        gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = 2;
        gi2cmsg.buf   = bTxbuff;       

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't write data \n");
           return -1;
       }              
    }
   
    else if (bCmd == PAGEWRITE)
    {
        if (uLen > 8)
       {  
           printf("invaild length\n");
           return -1;
        }
       
        bTxbuff[0] = bAddr; // set word address
        for (bi = 0 ; bi < uLen ; bi++)
        {
            bTxbuff[bi + 1] = *(pbData + bi);
        }   
       
        gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = uLen + 1;
        gi2cmsg.buf   = bTxbuff;  
       
        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't write data \n");
           return -1;
       }    
    }  
   
    else if (bCmd == RNDREAD)
    {
        bTxbuff[0] = bAddr; // word address
       
       gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = 1;
        gi2cmsg.buf   = bTxbuff;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't write word address \n");
           return -1;
       } 
       
        gi2cmsg.flags = I2C_M_RD;
       gi2cmsg.len   = 1;
       gi2cmsg.buf   = pbData;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't random read data \n");
           return -1;
       }   
    }
   
    else if (bCmd == SEQREAD)
    {
        bTxbuff[0] = bAddr; // word address
       
       gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = 1;
        gi2cmsg.buf   = bTxbuff;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't write word address \n");
           return -1;
       }       
       
        gi2cmsg.flags = I2C_M_RD;
       gi2cmsg.len   = uLen;
       gi2cmsg.buf   = pbData;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't sequential read data \n");
           return -1;
       }       
    } 
   
    else
    {
        printf("invaild command\n");  
        return -1;
    }
   
    return 0;
}

