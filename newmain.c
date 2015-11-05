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

#define OUTPUT 1 //0 = TEXT 1 = BIN
#define SENSE_INTERVAL 2000 //Interval to wait between readings

static const char *ctemp = "\n\rTemperature: ";
static const char *chum = "\n\rRelative humidity: ";
static const char *sht_status = "\n\rStatus byte: ";
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
    {putch(0x97); 
    test++;
    }

    while(1) {
      //  CLRWDT();
        
       
        
        if (OUTPUT == 0){
            sh = Sensor_read(READ_TEMP,2);   

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
            putch(0xFF);
            putch(0xFF);
            t = DegreesC(sh.sensor_val);
            putch(t >> 8);
            putch(t & 255);
            hum = Sensor_read(READ_HUMIDITY,2);
            h = HumidityPercent(hum.sensor_val,sh.sensor_val);
            putch(h >> 8);
            putch(h & 255);
            putch(Sensor_read(READ_STATUS,1).sensor_val & 255);
            putch(0x00); //For future outdoorsie temperature
            putch((sh.crc_ok << 1 ) + hum.crc_ok);
            
            
         
        }
            
          __delay_ms(SENSE_INTERVAL);        
    }
    
    return;
        
}
