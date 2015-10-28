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

#define OUTPUT 1 //0 = TEXT 1 = BIN


static const char *ctemp = "\n\rCurrent temperature: ";
static const char *chum = "\n\rCurrent humidity (compensated): ";
static const char *sht_status = "\n\rSHT7x sensor status: ";
static const char *circ = " CRC OK";


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

void interrupt tc_int(void) {
    
    unsigned int i = 0;
    if(PIR1bits.TMR1IF) 
    {

        PIR1bits.TMR1IF = 0;
        cnt++;
        
    }
          
        
        }
        
void setser(void)
{
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;
   // SPBRG = 51; //1200 @ 4MHZ
    SPBRG = 25; //9600 @ 4
    BAUDCTLbits.BRG16 = 1; // 0 FOR 1200
}

void main(void) {
    OSCCON = 0b11100001;
    Sht_rtn sh;
    Sht_rtn hum;
    int t;
    int h;
    
    int test = 0;
    
    TRISA = 0x00;
    ANSEL = 0x00;
    
    setser();
    
    TRISC = 0b00110000;
    //Timer
    INTCON = 0xC0;
    PIE1bits.TMR1IE = 1;
    
    TMR1L = 0X01;
    TMR1H = 0X00;
    T1CON = 0x35; //timer1 on
    CMCON0 = 0b00000111;
        
    while(test < 1000)
    {UART_Write(0x97); 
    test++;
    }

    while(1) {
        CLRWDT();
        
       
        
        if (OUTPUT == 0){
            sh = Sensor_read(READ_TEMP,2);   
            //cnt = 0;
            UART_Const(ctemp);
            t = sh.sensor_val;
            UART_Temp(t,33);
            
            if(sh.crc_ok){
            UART_Const(circ);
            }
            
            
            UART_Const(chum);
            sh = Sensor_read(READ_HUMIDITY,2);
            CalcHumidity(sh.sensor_val,t);
            UART_Const(sht_status);
            zero_b(Sensor_read(READ_STATUS,1).sensor_val & 255);
        }
        
        if(OUTPUT == 1)
        {
            sh = Sensor_read(READ_TEMP,2);
            UART_Write(0xFF);
            UART_Write(0xFF);
            t = DegreesC(sh.sensor_val);
            UART_Write(t >> 8);
            UART_Write(t & 255);
            hum = Sensor_read(READ_HUMIDITY,2);
            h = HumidityPercent(hum.sensor_val,sh.sensor_val);
            UART_Write(h >> 8);
            UART_Write(h & 255);
            UART_Write(Sensor_read(READ_STATUS,1).sensor_val & 255);
            UART_Write(0x00); //For future outdoorsie temperature
            UART_Write((sh.crc_ok << 1 ) + hum.crc_ok);
            
            
         
        }
            
          __delay_ms(2000);        
    }
    
    return;
        
}
