#include <pic16f688.h>
#include <xc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "sht7x.h"

//Sht75 constants for relative humidity
const float RHc = -1.5955E-6;
const float RHc2 = 0.00008;
const float RHc0 = -2.0468;
const float RHc1 = 0.0367;


char ReadByte(char getCRC){
    //TRISC must be 0b00110000 before using this
    char c;
    char rcv;
    char ack = 0;
    
    for(c=0;c<8;c++){
        PORTCbits.RC1 = 1;
        __delay_us(READ_WAIT);     
        if(c<8) {rcv = (rcv <<1)  + (PORTC & 1);}
        else{ack = PORTC & 1;}
               
        
        PORTCbits.RC1 = 0;
        __delay_us(READ_WAIT);
}    
    if(getCRC){
             TRISCbits.TRISC0 = 0;
             PORTCbits.RC0 = 0;
             __delay_us(8);
             PORTCbits.RC1 = 1;
             __delay_us(READ_WAIT);
             PORTCbits.RC1 = 0;
                PORTCbits.RC0 = 0;
            TRISCbits.TRISC0 = 1;
            __delay_us(12);
    }        
   
    return rcv;
    
}



void SendACK(){
    TRISCbits.TRISC0 = 0;
    PORTCbits.RC0 = 0;
    __delay_us(8);
    PORTCbits.RC1 = 1;
    __delay_us(READ_WAIT);
    PORTCbits.RC1 = 0;
    PORTCbits.RC0 = 0;
    TRISCbits.TRISC0 = 1;
    __delay_us(12);
}


void SupSeq(void){
    //Shit75 startup sequence
            //0=data 1=clock
        PORTC = 0b00000000;
        __delay_us(2);
        PORTC = 0b00000001;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000011;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000010;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000000;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000010;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000011;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000001;
        __delay_us(SUP_WAIT);
        PORTC = 0b00000000;
}

void SendByte(char byt) {
    char ack;
    char c;
    for(c=0;c<8;c++){
        if ((byt & 0x80) != 0) {
        PORTCbits.RC0 = 1;
        __delay_us(10);
        PORTCbits.RC1 = 1;
        __delay_us(READ_WAIT);
        PORTC = 0b00000001;
        __delay_us(READ_WAIT);
       
        
        }
        else{
        
        PORTC = 0b00000010;
        __delay_us(READ_WAIT);
        PORTC = 0b00000000;
        __delay_us(READ_WAIT);
            
        }
        byt = byt << 1;
        //Don't bother dropping data line low if next bit is also a 1
        if((byt & 0x80) == 0) {PORTC = 0b00000000;}
        
    }
 //Check for ACK
    
    // Read ack bit
    TRISC = 0b00110001;
    
    PORTC = 0b00000010;
    __delay_us(READ_WAIT);
   ack = PORTC & 1;
    PORTC = 0b00000000;
    
    //Send TRISC back to original state both pins write
   // TRISC = 0b00110000;
 
}



signed int DegreesC(int sensorval){
    return (T_OFFSET + sensorval);
    
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



//Needed for printf
void putch(char data)
{
  while(!TRMT);
  TXREG = data;
}

void UART_String(char* letters) {
    int i = 0;
    while(letters[i] != 0) {
        putch(letters[i++]);
    }
}


void UART_Const(const char* letters) {
    int i = 0;
    while(letters[i] != 0) {
        putch(letters[i++]);
    }
}


void UART_Temp(int sensorval,char volt) {
    //Needs - sign for temp between -1 and 0;
    
    char buf[5];
    int offset;
    int deg;
    int fraction;
    
    deg = (sensorval + T_OFFSET) / 100;
    fraction = abs((sensorval + T_OFFSET) % 100);
    
    itoa(buf,deg,10);
    UART_String(buf);
    putch(46); //Decimal point
    itoa(buf,fraction,10);
    UART_String(buf);
    putch(248);
}

void CalcHumidity(int sensorval,int tempval){
    char buf[5];
    int rh = HumidityPercent(sensorval,tempval);
    itoa(buf, (int) rh/10,10);
    UART_String(buf);
}

int HumidityPercent(int sensorval,int tempval){
        float rh = RHc0 + (RHc1 * sensorval) + ((RHc * sensorval * sensorval));
        float rht;
        float truetemp = (T_OFFSET +  tempval) / 100;
        //Temperature compensate        
        rh = (truetemp - 25)*(0.01 + RHc2 * sensorval) + rh;
        rh = rh*10;
        return (int) rh;
}


void zero_b(char bt){
    char lp;
    putch(48);
    putch(98);
    for(lp=8;lp>0;lp--){
        if((bt & (1<<lp-1)) != 0){
            putch(49);
        }
        else{putch(48);}
    }
}


void Set_Settings(char setts) {
    //Be careful, may not work and may put sensor into 8/10bit mode
    long wait_val = 0;
        SupSeq();
        SendByte(0b00000110);      
      __delay_us(1);
    //while((PORTC & 1) == 1) {
    //   wait_val++;
    //}
        SendByte(setts);       
        TRISC = 0b00110000;
        __delay_ms(10);
}


Sht_rtn Sensor_read(char val,char bytes){
    //Val = command to send, bytes = number of bytes to read (can only be 1 or 2)
    Sht_rtn rval;
    rval.wait_val = 0;
    
    char th = 0;
    char tl = 0;
    rval.crc_generated = 0;
    
    unsigned int ix;
    char revCRC = 0;
    
    //Add command to crc
    doCRC(val,&rval.crc_generated);
    
    //Send command
    SupSeq(); //Start up sequence
    SendByte(val);
   
   
    __delay_us(READ_WAIT);
    //Wait for line to be pulled low
    while(((PORTC & 1) == 1) && rval.wait_val < SHT_TIMEOUT) {
       rval.wait_val++;
    }
    
    if(bytes == 2){
        th=ReadByte(1);
        doCRC(th,&rval.crc_generated);
    }
    
    tl=ReadByte(1);
   
    rval.crc_received=ReadByte(0);
     TRISC = 0b00110000;
     
  
 //CRC Stuff    
   doCRC(tl,&rval.crc_generated);
   for (ix = 0; ix < 8; ix++) {
      if ((0x80 >> ix) & rval.crc_received)
        revCRC |= (1 << ix);
    }
  rval.crc_received = revCRC;
  if (rval.crc_generated == rval.crc_received) { rval.crc_ok = 1;}
  
  rval.sensor_val = (th<<8) + tl; 
  return rval;
}