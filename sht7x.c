#include <pic16f688.h>
#include <xc.h>
#include <stdlib.h>
#define _XTAL_FREQ 8000000     
#define CRC_POLY        0x31            /* CRC polynomial x**8 + x**5 + x**4 */


void SendACK(){
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

void w1(char dly){
    char i=0;
    while(i<dly){
    i++;}
    return;
}

void wastetime(int dely){
    long b;
    long a;
    for(b = 0;b < 8000*dely;b++){
        a = b;
    }
}

void SupSeq(void){
    //Shit75 startup sequence
            //0=data 1=clock
        PORTC = 0b00000000;
        __delay_us(2);
        PORTC = 0b00000001;
        __delay_us(10);
        PORTC = 0b00000011;
        __delay_us(10);
        PORTC = 0b00000010;
        __delay_us(10);
        PORTC = 0b00000000;
        __delay_us(10);
        PORTC = 0b00000010;
        __delay_us(10);
        PORTC = 0b00000011;
        __delay_us(10);
        PORTC = 0b00000001;
        __delay_us(10);
        PORTC = 0b00000000;
}

signed int DegreesC(int sensorval){
    int offset = -4010; //For 5V
    return offset + sensorval;
    
}


/**
 * DegreesAsc
 * Convert 16-bit int from sensor into degrees celsius in ASCII
 *
 * 
 */

void DegreesAsc(int sensorval,char* buf,char volt) {
    int offset;
    if(volt == 50) { offset = -4010;} //For 5V
    else
    { offset = -3970;}
    
    itoa(buf,sensorval + offset,10);
    
}



/**
 * doCRC
 * <<<This function is shamelessly copied from elsewhere>>>
 *      Routine to calculate the CRC while message is sent / received
 *
 * Parameters:
 *      @ch             character to be added to CRC
 *      @crc            crc to which character is to be added
 *
 * Returns:
 *      Target CRC value is updated
 */
void doCRC (unsigned char ch, unsigned char *crc)
{
    int ix;
    unsigned char b7;
    
    for (ix = 0; ix < 8; ix++) {
        b7 = ch ^ *crc;
        *crc <<= 1;
        ch <<= 1;
        if (b7 & 0x80)
            *crc ^= CRC_POLY;
    }
    return;
}