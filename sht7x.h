/* 
 * File:   sht7x.h
 * Author: daniel
 *
 * Created on 02 October 2015, 21:43
 */

#ifndef SHT7X_H
#define	SHT7X_H
#define _XTAL_FREQ 8000000     

#define CRC_POLY        0x31            /* CRC polynomial x**8 + x**5 + x**4 */
#define SUP_WAIT        10      //Number of uS to wait between startup sequence steps
#define SHT_TIMEOUT     800000 //Number of iterations to wait for SHT to return command
#define READ_WAIT       20        //Number of uS between read states

//#define T_OFFSET = -4010;
#define T_OFFSET        -3970







const char READ_TEMP = 0b00000011;
const char READ_HUMIDITY = 0b00000101;
const char READ_STATUS = 0b00000111;
const char READ_RESET = 0b00011110;

typedef struct sht_return
{
    int sensor_val;
    
    char crc_generated;
    char crc_received;
    char crc_ok;
    
    char in_cmd;
    char bytecount;
    long wait_val;
    char error;
} Sht_rtn;


unsigned int sht_read(char,char);
void SendByte(char);
char ReadByte(char);
void SupSeq(void);
void SendACK(void);
void doCRC(char,char*);
void CalcHumidity(int,int);
int HumidityPercent(int,int);
int DegreesC(int);

void zero_b(char);
void Set_Settings(char);
void putch(char);
void UART_String(char*);
void UART_Const(const char*);
void UART_Temp(int,char);
Sht_rtn Sensor_read(char,char);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SHT7X_H */

