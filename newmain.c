/*
 * File:   newmain.c
 * Author: daniel
 *
 * Created on 07 September 2015, 23:05
 */


#include <xc.h>
#include <stdio.h>




#include "sht7x.h"
#include <pic16f688.h>
int cnt = 0;
int upto = 3;

char CRCr = 0;
char CRCg = 0;

static const char *ctemp = "Current temperature: ";
static const char *chum = "Current humidity: ";
static const char *sht_status = "SHT7x sensor status: ";

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)



void interrupt tc_int(void)

{
    unsigned int i = 0;
    if(PIR1bits.TMR1IF) 
    {

        PIR1bits.TMR1IF = 0;
        cnt++;

        

    }
        
        
        
        }
        






void setser(void)
{
    return;
}




unsigned int sht_read(char val){
    long wait_val = 0;
    
    char th;
    char tl;
    char crc;
    char received_crc;
    
    
    
    //Add command to crc
    doCRC(val,&crc);
    //Send command
    SendByte(val,20);
    

    
    
   
    
    __delay_us(20);
    //Wait for line to be pulled low
    while((PORTC & 1) == 1) {
       wait_val++;
      //  UART_Write(119);
      // if(wait_val > 50000) { return 0;}
    }
   
    //__delay_ms(20);
    
    th=ReadByte(1);
    tl=ReadByte(1);
   
    received_crc=ReadByte(1);

    doCRC(th,&crc);
    doCRC(tl,&crc);
    
    TRISC = 0b00110000;
    
    CRCr = received_crc;
    CRCg = crc;
    return (th<<8) + tl;
}





char ReadByte(char getCRC){
    //TRISC must be 0b00110000 before using this
    char c;
    char rcv;
    char ack = 0;
    //test
    //if((PORTC & 1) ==1) {return 0xFF;}
    //else{return 0x00;}
    
    
    for(c=0;c<8;c++){
        PORTCbits.RC1 = 1;
        __delay_us(20);     
        if(c<8) {rcv = (rcv <<1)  + (PORTC & 1);}
        else{ack = PORTC & 1;}
               
        
        PORTCbits.RC1 = 0;
        __delay_us(20);
}    
    if(getCRC){
             TRISCbits.TRISC0 = 0;
             PORTCbits.RC0 = 0;
             __delay_us(8);
             PORTCbits.RC1 = 1;
             __delay_us(20);
             PORTCbits.RC1 = 0;
                PORTCbits.RC0 = 0;
            TRISCbits.TRISC0 = 1;
            __delay_us(12);
    }        
   
    return rcv;
    
}


void main(void) {
    OSCCON = 0b11100001;
    unsigned int i;
    int cs;
    char tsend;
    char th = 0;
    char tl = 0;
    long waste_time = 0;  
    TRISA = 0x00;
    ANSEL = 0x00;
    
    TRISC = 0b00110000;
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;
    SPBRG = 51; //1200 @ 4MHZ
    BAUDCTLbits.BRG16 = 0;
    //Timer
    INTCON = 0xC0;
    PIE1bits.TMR1IE = 1;
    
    TMR1L = 0X01;
    TMR1H = 0X00;
    T1CON = 0x35; //timer1 on
    CMCON0 = 0b00000111;
    
   
        

         
 /*   while(1){
            SupSeq();
            i = sht_read(READ_TEMP); 
            __delay_ms(50);
        
    }
*/


        
     while(1) {
        CLRWDT();
            SupSeq();
            i = sht_read(READ_TEMP);   
            //cnt = 0;
            UART_Const(ctemp);
            UART_Temp(i,33);
            if(CRCr == CRCg) { UART_Write(33);}
            else{UART_Write(63);}
            UART_Write(10);
            UART_Write(13);
            UART_Const(chum);
            SupSeq();
            i = sht_read(READ_HUMIDITY);
            CalcHumidity(i);
            UART_Write(10);
            UART_Write(13);
            UART_Const(sht_status);
            UART_Write(10);
            UART_Write(13);
            //SendByte(READ_RESET,20);
    __delay_ms(1000);                
    }
    
    
    return;
        
}
