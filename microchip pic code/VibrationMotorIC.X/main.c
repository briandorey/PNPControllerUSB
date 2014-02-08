/* 
 * File:   main.c
 * Author: andrew
 *
 * Created on 08 January 2014, 16:09
 */




#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <pic12f1822.h>
#include <htc.h>


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// Define Clock
 #define _XTAL_FREQ  16000000        // this is used by the __delay_ms(xx) and __delay_us(xx) functions


/* Bit defines */
//------------------------
#define ADDRESS  0x16          // Set address here!!!!!!!!!!!
//------------------------
#define TRUE                  1
#define FALSE                 0
#define LED                     RA5
#define FOUND            0x06
#define NOT_FOUND         0x07
#define SENSOR_OK          0x03


#define PIC_SSPSTAT_BIT_BF      0x01
#define PIC_SSPCON1_BIT_CKP     0x10
#define PIC_SSPCON1_BIT_WCOL    0x80
#define RX_BUF_LEN              8
#define RELEASECLK              SSP1CON1bits.CKP

/** I N T E R R U P T S **********************************************/
unsigned char CompareState(void);
void WriteI2C(unsigned char transmitByte);


/** D E C L A R A T I O N S *******************************************/
unsigned char sendBuffer[RX_BUF_LEN];
unsigned char receiveBuffer[8];
char bufferIndex;




void init(){

    // set up oscillator control register
    OSCCONbits.SPLLEN=0;    // PLL is disabled
    OSCCONbits.IRCF=0x0F;   //set OSCCON IRCF bits to select OSC frequency=16Mhz
    OSCCONbits.SCS=0x02;    //set the SCS bits to select internal oscillator block

    // Set up I/O pins
    ANSELAbits.ANSELA=0;    // set all analog pins to digital I/O
    ADCON0bits.ADON=0;      // turn ADC off
    DACCON0bits.DACEN=0;    // turn DAC off

    // PORT A Assignments
    TRISAbits.TRISA0 = 0;	// RA0 = nc
    TRISAbits.TRISA1 = 1;	// RA1 = I2C Clock
    TRISAbits.TRISA2 = 1;	// RA2 = I2C Data
    TRISAbits.TRISA3 = 0;	// RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0;	// RA4 = nc
    TRISAbits.TRISA5 = 0;	// RA5 = PWM Output (CCP1) connected to LED

    APFCONbits.CCP1SEL = 1;       // The CCP1SEL bit selects which pin the PWM output is on.
                                // The default value for this bit is 0 which sets the
                                // PWM output on RA2.  If you want the PWM output on
                                // RA5 instead then set this bit to a 1.

    // TMR2 Prescalar=1: PWM Period = (1/16000000)*4*1*256 = 64 us or 15.63 khz

    // ***** Setup PWM output ******************************************************

    TRISAbits.TRISA5 = 1;       // disable pwm pin output for the moment

    CCP1CONbits.CCP1M=0x0C;     // select PWM mode for CCP module
    CCP1CONbits.P1M=0x00;	// select single output on CCP1 pin (RA5)

    PR2 = 0xff;                 // set PWM period as 255 per our example above

    CCPR1L =  0x00;             // clear high 8 bits of PWM duty cycle
    CCP1CONbits.DC1B=0x00;	// clear low 2 bits of PWM Duty cycle

                                // Note: PWM uses TMR2 so we need to configure it
    PIR1bits.TMR2IF=0;		// clear TMR2 interrupt flag
    T2CONbits.T2CKPS=0x00;      // select TMR2 prescalar as divide by 1 as per our example above
    T2CONbits.TMR2ON=1;		// turn TMR2 on

    

    TRISAbits.TRISA5 = 0;	// turn PWM output back on

    // ****** Setup Interupts ******************************************************

    INTCONbits.GIE = 1;                 // Enable global interrupts
    INTCONbits.PEIE = 1;                // Enable Peripheral interrupts
    PIE1bits.SSP1IE = 1;                // Enable I2C interrupt
    PIR1bits.SSP1IF = 0;

    // ****** Setup I2C ******************************************************
    SSP1CON1 = 0b00110110;
    SSP1CON2 = 0b00000001;
    SSP1CON3 = 0b00010000;
    SSP1STAT = 0b00000000;
    SSP1ADD = ADDRESS;
}

void SetPWMDutyCyle(char duty_cycle_value)
{
    unsigned int duty_cycle;
    duty_cycle = duty_cycle_value * 4;

    CCP1CONbits.DC1B = duty_cycle & 0x03; //first set the 2 lsb bits
    CCPR1L =  (duty_cycle >> 2);           //now set upper 8 msb bits
}

void startPWM(){
    TRISAbits.TRISA5 = 0;
}

void stopPWM(){
    TRISAbits.TRISA5 = 1;
}

void WriteI2C(unsigned char transmitByte){
    unsigned char writeCollision = 1;

    while (SSP1STAT & PIC_SSPSTAT_BIT_BF);         // Is BF bit set in PIC_SSPSTAT?
    while (writeCollision){
        SSP1CON1 &= ~PIC_SSPCON1_BIT_WCOL;          // Clear the WCOL flag
        SSP1BUF = transmitByte;
        if (SSP1CON1 & PIC_SSPCON1_BIT_WCOL)
            writeCollision = 1;
        else
            writeCollision = 0;
    }
    SSP1CON1 |= PIC_SSPCON1_BIT_CKP;              // Release the clock.
}

static void interrupt isr(void){
    if(PIR1bits.SSP1IF == TRUE){
        PIR1bits.SSP1IF = 0;
        unsigned char i2c_mask = 0x2D;          // 0010 1101
        unsigned char tempSSPSTAT;
        unsigned char temp;
        tempSSPSTAT = SSP1STAT & i2c_mask;

        switch (tempSSPSTAT){
    /* Write operation, last byte was an address, buffer is full */
            case 0x09:                               // 0000 1001
                bufferIndex = 0;                      // Clear the buffer index
                temp = SSP1BUF;                        // Do a dummy read of PIC_SSPBUF
                RELEASECLK = 1;
                break;

    /* Write operation, last byte was data, buffer is full */
            case 0x29:                                          // 0010 1001
                receiveBuffer[bufferIndex] = SSP1BUF;                 // Get the byte from the SSP
                bufferIndex++;                                 // Increment the buffer pointer
                if(bufferIndex >= 7)
                   bufferIndex = 0;                            // Yes, clear the buffer index.
                RELEASECLK = 1;
                break;

    /* Read operation; last byte was an address, buffer is empty */
            case 0x0D:                                          // 0000 1100
                temp = SSP1BUF;
                bufferIndex = 0;                               // Clear the buffer index
                WriteI2C(sendBuffer[bufferIndex]);          // Write the byte to PIC_SSPBUF
                bufferIndex++;                                 // increment the buffer index
                RELEASECLK = 1;
                break;

    /* Read operation; last byte was data, buffer is empty */
            case 0x2D:                                          // 0010 1100
                if(bufferIndex >= RX_BUF_LEN)
                    bufferIndex = 0;                           // Yes, clear the buffer index
                WriteI2C(sendBuffer[bufferIndex]);          // Write to PIC_SSPBUF
                bufferIndex++;                                 // increment the buffer index
                RELEASECLK = 1;
                break;
        }
    }
}

int main(int argc, char** argv) {

    init();
    SetPWMDutyCyle(100);
    stopPWM();
    do{
        if(receiveBuffer[0] == 0x01){
            startPWM();
            receiveBuffer[0] = 0x00;
        }
        if(receiveBuffer[0] == 0x02){
            stopPWM();
            receiveBuffer[0] = 0x00;
        }
        if(receiveBuffer[0] == 0x03){
            SetPWMDutyCyle(receiveBuffer[1]);
            //sendBuffer[0] = 1;
            receiveBuffer[0] = 0x00;
        }

    } while (1);

    return (EXIT_SUCCESS);
}

