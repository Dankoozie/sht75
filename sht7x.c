#include <pic16f688.h>
#include <xc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define _XTAL_FREQ 8000000     
#define CRC_POLY        0x31            /* CRC polynomial x**8 + x**5 + x**4 */

const float RHc = -1.5955E-6;


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

void SendByte(char byt, char wt) {
    char pc = byt;
    char ack;
    char c;
    for(c=0;c<8;c++){
        if ((pc & 0x80) != 0) {
        PORTCbits.RC0 = 1;
        __delay_us(10);
        PORTCbits.RC1 = 1;
        __delay_us(20);
        PORTC = 0b00000001;
        __delay_us(20);
       
        
        }
        else{
        
        PORTC = 0b00000010;
        __delay_us(20);
        PORTC = 0b00000000;
        __delay_us(20);
            
        }
        pc = pc << 1;
        //Don't bother dropping data line low if next bit is also a 1
        if((pc & 0x80) == 0) {PORTC = 0b00000000;}
        
    }
 //Check for ACK
    
    // Read ack bit
    TRISC = 0b00110001;
    
    PORTC = 0b00000010;
    __delay_us(20);
   ack = PORTC & 1;
    PORTC = 0b00000000;
    
    //Send TRISC back to original state both pins write
   // TRISC = 0b00110000;
 
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

void DegreesAsc(int sensorval,char* degrees, char* frac,char volt) {
    int offset;
    int deg;
    int fraction;
    if(volt == 50) { offset = -4010;} //For 5V
    else
    { offset = -3970;} //For 3.3v
    deg = (sensorval + offset) / 100;
    fraction = (sensorval + offset) % 100;
    
    itoa(degrees,deg,10);
    itoa(frac,fraction,10);
    
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




void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}


void UART_String(char* letters) {
    int i = 0;
    while(letters[i] != 0) {
        UART_Write(letters[i++]);
    }
}


void UART_Const(const char* letters) {
    int i = 0;
    while(letters[i] != 0) {
        UART_Write(letters[i++]);
    }
}


void UART_Temp(int sensorval,char volt) {
    //Needs - sign for temp between -1 and 0;
    
    char buf[5];
    int offset;
    int deg;
    int fraction;
    
    if(volt == 50) { offset = -4010;} //For 5V
    else
    { offset = -3970;} //For 3.3v
    deg = (sensorval + offset) / 100;
    fraction = abs((sensorval + offset) % 100);
    
    itoa(buf,deg,10);
    UART_String(buf);
    UART_Write(46); //Decimal point
    itoa(buf,fraction,10);
    UART_String(buf);
    UART_Write(248);
}

int CalcHumidity(int sensorval,int tempval){
    float rh; 
    char buf[10];
    
    rh = -2.0468 + (0.0367 * sensorval) + ((RHc * sensorval)*(RHc * sensorval));
    
    //Temperature compensate
    rh = ((tempval / 100) - 25) * (0.01 + (0.00008 * sensorval)) + rh;
    
    
    itoa(buf, (int) rh,10);
    
    UART_String(buf);
    
    return 0;
}

void zero_b(char bt){
    char lp;
    UART_Write(48);
    UART_Write(98);
    for(lp=8;lp>0;lp--){
        if((bt & (1<<lp-1)) != 0){
            UART_Write(49);
        }
        else{UART_Write(48);}
    }
}
/*
char sensor_status(void) {
    long wait_val = 0;
    char crc;
    char stat;
    SupSeq();
    SendByte(0b00000111,20);
    
            
     __delay_us(60);
    //Wait for line to be pulled low
    while((PORTC & 1) == 1) {
       wait_val++;
    }
     
    stat=ReadByte(1);
    crc=ReadByte(1);

    TRISC = 0b00110000;
    
     return stat;
}
*/

void Set_Settings(char setts) {
    long wait_val = 0;
        SupSeq();
        SendByte(0b00000110,20);
         
      __delay_us(1);
    //while((PORTC & 1) == 1) {
    //   wait_val++;
    //}
        SendByte(setts,20);
        
        TRISC = 0b00110000;

        __delay_ms(10);
}