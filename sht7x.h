/* 
 * File:   sht7x.h
 * Author: daniel
 *
 * Created on 02 October 2015, 21:43
 */

#ifndef SHT7X_H
#define	SHT7X_H

#define _XTAL_FREQ 8000000     
const char READ_TEMP = 0b00000011;
const char READ_HUMIDITY = 0b00000101;
const char READ_STATUS = 0b00000111;
const char READ_RESET = 0b00011110;

unsigned int sht_read(char);
void SendByte(char,char);
char ReadByte(char);
void SupSeq(void);
void w1(char);
void wastetime(int);
void SendACK(void);
//void DegreesAsc(int,char*,char*,char);
void doCRC(char,char*);
int CalcHumidity(int);



void UART_Write(char);
void UART_String(char*);
void UART_Const(const char*);
void UART_Temp(int,char);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SHT7X_H */

