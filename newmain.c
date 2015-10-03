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

void UART_Write(char);


void interrupt tc_int(void)

{
    unsigned int i = 0;
    if(PIR1bits.TMR1IF) 
    {

        PIR1bits.TMR1IF = 0;
        cnt++;

        

    }
        
        
        
        }
        







void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}

void setser(void)
{
    return;
}




unsigned int sht_read(char val){
    long wait_val = 0;
    
    char ack;
    char th;
    char tl;
    char crc;
    //Send command
    SendByte(val,20);
    
    //Read ack bit
    TRISC = 0b00110001;
    
    PORTC = 0b00000010;
    w1(3);
    ack = PORTC & 1;
    PORTC = 0b00000000;
    
    __delay_us(100);
    //Wait for line to be pulled low
    while((PORTC & 1) == 1) {
        wait_val++;
    }
    
    
    th=ReadByte();
    tl=ReadByte();
    crc=ReadByte();

    
    TRISC = 0b00110000;
    return (th<<8) + tl;
}




void SendByte(char byt, char wt) {
    char pc = byt;
    char c;
    for(c=0;c<8;c++){
        if ((pc & 0x80) != 0) {
        PORTCbits.RC0 = 1;
        __delay_us(10);
        PORTCbits.RC1 = 1;
        __delay_us(20);
        PORTC = 0b00000001;
        __delay_us(20);
        PORTC = 0b00000000;
        
        }
        else {
        PORTC = 0b00000010;
        __delay_us(20);
        PORTC = 0b00000000;
        __delay_us(20);
            
        }
        pc = pc << 1;
    }
 //Check for ACK
}

char ReadByte(){
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
    
             TRISCbits.TRISC0 = 0;
             PORTCbits.RC0 = 0;
             __delay_us(8);
             PORTCbits.RC1 = 1;
             __delay_us(20);
             PORTCbits.RC1 = 0;
                PORTCbits.RC0 = 0;
            TRISCbits.TRISC0 = 1;
            __delay_us(12);
            
   
    return rcv;
    
}


void main(void) {
    char star[10];
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
    
   
        
            
        



        
     while(1) {
        CLRWDT();
            SupSeq();
            i = sht_read(READ_TEMP);   
            //cnt = 0;
            DegreesAsc(i,star,33);
            UART_Write(106);
            UART_Write(star[0]);
            UART_Write(star[1]);
            UART_Write(star[2]);
            UART_Write(star[3]);
            UART_Write(10);
            UART_Write(13);
            
            
                    
    __delay_ms(1000);                
    }
    
    
    return;
        
}
