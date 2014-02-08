
#ifndef MAIN_C
#define MAIN_C
#define USE_OR_MASKS

// Global includes
// Note: string.h is required for sprintf commands for debug
#include <string.h>
#include <pwm.h>
#include <timers.h>
#include <EEP.h>
#include <delays.h>
#include "i2c.h"

// Local includes
#include "HardwareProfile.h"



// Microchip Application Library includes
// (expects V2.9a of the USB library from "Microchip Solutions v2011-07-14")
//
// The library location must be set in:
// Project -> Build Options Project -> Directories -> Include search path
// in order for the project to compile.
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Define the globals for the USB data in the USB RAM of the PIC18F*550
#pragma udata
#pragma udata USB_VARIABLES=0x500
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
#pragma udata

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = FLAG_TRUE;

// Define the application variables

unsigned char vac1running = 0;
unsigned char vac2running = 0;
unsigned char led1running = 0;
unsigned char led2running = 0;
unsigned char vibrationrunning = 0;

// pwm values
unsigned int led1_duty_cycle = 0;
unsigned int led2_duty_cycle = 0;
unsigned int vibrationmotor_duty_cycle = 0;

unsigned int headLED_EEPROM_address = 0x0200;
unsigned int baseLED_EEPROM_address = 0x0202;

// i2c variables
unsigned char feederid;


// PIC18F4550/PIC18F2550 configuration for the WFF Generic HID test device
#pragma config PLLDIV   = 5         // 20Mhz external oscillator
#pragma config CPUDIV   = OSC1_PLL2   
#pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
#pragma config FOSC     = HSPLL_HS
#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config BOR      = ON
#pragma config BORV     = 3
#pragma config VREGEN   = ON
#pragma config WDT      = OFF
#pragma config WDTPS    = 32768
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config STVREN   = ON
#pragma config LVP      = OFF
#pragma config XINST    = OFF
#pragma config CP0      = OFF
#pragma config CP1      = OFF
#pragma config CPB      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
#pragma config WRTB     = OFF
#pragma config WRTC     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
#pragma config EBTRB    = OFF

// Private function prototypes
static void initialisePic(void);
void processUsbCommands(void);
void applicationInit(void);
void USBCBSendResume(void);
void highPriorityISRCode();
void lowPriorityISRCode();

// Remap vectors for compatibilty with Microchip USB boot loaders
#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018


extern void _startup (void);
#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
void _reset (void)
{
   _asm goto _startup _endasm
}

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR (void)
{
     _asm goto highPriorityISRCode _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR (void)
{
     _asm goto lowPriorityISRCode _endasm
}

#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR (void)
{
     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR (void)
{
     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}


#pragma code

// High-priority ISR handling function
#pragma interrupt highPriorityISRCode
void highPriorityISRCode()
{
	// Application specific high-priority ISR code goes here
	
	#if defined(USB_INTERRUPT)
		// Perform USB device tasks
		USBDeviceTasks();
	#endif

}

// Low-priority ISR handling function
#pragma interruptlow lowPriorityISRCode
void lowPriorityISRCode()
{
	// Application specific low-priority ISR code goes here
}

// String for creating debug messages
char debugString[64];

// Main program entry point
void main(void)
{   
	// Initialise and configure the PIC ready to go
    initialisePic();



	// If we are running in interrupt mode attempt to attach the USB device
    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif
	

	// Main processing loop
    while(1)
    {
        #if defined(USB_POLLING)
			// If we are in polling mode the USB device tasks must be processed here
			// (otherwise the interrupt is performing this task)
	        USBDeviceTasks();
        #endif
    	
    	// Process USB Commands
        processUsbCommands();  
        
        // Note: Other application specific actions can be placed here      
    }
}

// Initialise the PIC
static void initialisePic(void)
{
    // PIC port set up --------------------------------------------------------

	// Default all pins to digital
    ADCON1 = 0x0F;

    // Configure ports as inputs (1) or outputs(0)
    TRISA = 0b00000000;
    TRISB = 0b00000000;
    TRISC = 0b00000000;
    TRISD = 0b00000000;
    TRISE = 0b00000000;

    // Clear all ports
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    PORTC = 0b00000000;
    PORTD = 0b00000000;
    PORTE = 0b00000000;

    // initialise the atmega IC
    atmegaFeederRunningTRIS = 1;
    atmegaResetPin = 1;
    
    
    // initialise i2c communication
    OpenI2C( MASTER, SLEW_OFF);
    SSPADD = 0x70;
    
    // Application specific initialisation
    applicationInit();
    
    // Initialise the USB device
    USBDeviceInit();

    // Initialise the output pins
    setVac1off;
    setVac2off;
    setVibrationoff;
    OpenTimer2( TIMER_INT_OFF & T2_PS_1_1);

    // load pwm values from eeprom
    led1_duty_cycle = Read_b_eep(baseLED_EEPROM_address);
    led2_duty_cycle = Read_b_eep(headLED_EEPROM_address);

    // initialise the stepper driver and start timer 0 at 1 microsecond intervals
    
    
}



// Application specific device initialisation
void applicationInit(void)
{
	
    // Initialize the variable holding the USB handle for the last transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}

// Process USB commands
void processUsbCommands(void)
{   
    // Check if we are in the configured state; otherwise just return
    if((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))
    {
	    // We are not configured
	    return;
	}

	// Check if data was received from the host.
    if (!HIDRxHandleBusy(USBOutHandle)) {
        // Command mode
        switch (ReceivedDataBuffer[0]) {
            case 0x01: // System Commands
                switch (ReceivedDataBuffer[1]) {
                    case 0x01: // System Commands
                        // Copy any waiting debug text to the send data buffer
                        ToSendDataBuffer[0] = 0xFF;
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }
                        break;


                    default: // Unknown command received
                        break;
                }
                break;

            case 0x02: // Feeder Commands
                switch (ReceivedDataBuffer[1]) {
                    case 0x02: // Feeder Status
                        if (atmegaFeederRunning){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                            ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }
                        break;

                    case 0x03: // Go to feeder
                        StartI2C();
                        WriteI2C(0x28); // sends address to the device
                        IdleI2C();
                        WriteI2C(ReceivedDataBuffer[2]); // sends a control byte to the device
                        IdleI2C();
                        StopI2C();
                        break;

                    case 0x04: // Reset Feeder Z
                        StartI2C();
                        WriteI2C(0x28); // sends address to the device
                        IdleI2C();
                        WriteI2C(80); // sends a control byte to the device
                        IdleI2C();
                        StopI2C();
                        break;

                    case 0x05: // Full feeder reset
                        StartI2C();
                        WriteI2C(0x28); // sends address to the device
                        IdleI2C();
                        WriteI2C(70); // sends a control byte to the device
                        IdleI2C();
                        StopI2C();
                        break;
                     case 0x06: // Reset ATMEGA IC
                        atmegaResetPin = 0;
                        Delay1KTCYx(10);
                        atmegaResetPin = 1;
                        break;


                    default: // Unknown command received
                        break;
                        
                }
            break;
            case 0x03: // Vacuum and Vibration Commands
                switch (ReceivedDataBuffer[1]) {
                    case 0x01: // Vacuum 1 set
                        if (ReceivedDataBuffer[2] == 0x01){
                            setVac1on;
                        }
                        else{
                            setVac1off;
                        }
                        break;

                    case 0x02: // Vacuum 2 set
                        if (ReceivedDataBuffer[2] == 0x01){
                            setVac2on;
                        }
                        else{
                            setVac2off;
                        }
                        break;

                    case 0x03: // Vibration Motor set
                        if (ReceivedDataBuffer[2] == 0x01){
                           setVibrationon;
                            StartI2C();
                            WriteI2C(0x16); // sends address to the device
                            IdleI2C();
                            WriteI2C(0x01); // sends a control byte to the device
                            IdleI2C();
                            StopI2C();

                        }
                        else{
                           setVibrationoff;
                           StartI2C();
                            WriteI2C(0x16); // sends address to the device
                            IdleI2C();
                            WriteI2C(0x02); // sends a control byte to the device
                            IdleI2C();
                            StopI2C();
                        }
                        break;
                    case 0x04: // Vacuum 1 status
                        if (vac1running == 1){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                           ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }
                        break;
                    case 0x05: // Vacuum 2 status
                        if (vac2running == 1){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                           ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }
                        break;
                    case 0x06: // Vibration Motor status
                        if (vibrationrunning == 1){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                           ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }
                        break;
                    case 0x07: // Vibration Motor status
                        vibrationmotor_duty_cycle = ReceivedDataBuffer[2];
                        StartI2C();
                            WriteI2C(0x16); // sends address to the device
                            IdleI2C();
                            WriteI2C(0x03); // sends a control byte to the device
                            IdleI2C();
                            WriteI2C(vibrationmotor_duty_cycle); // sends a control byte to the device
                            IdleI2C();
                            StopI2C();
                        break;

                    default: // Unknown command received
                        break;
                        
                }
                break;
            case 0x04: // LED Commands
                switch (ReceivedDataBuffer[1]) {
                    case 0x01: // LED Base Camera on/off
                        if (ReceivedDataBuffer[2] == 0x01){
                        OpenPWM1(0xFF);
                        led1_duty_cycle = led1_duty_cycle * 4;
                        SetDCPWM1(led1_duty_cycle);
                        //    outBaseLED = 1;
                        led1running = 1;
                        }else{
                           ClosePWM1();
                           // outBaseLED = 0;
                           led1running = 0;
                        }
                        break;

                    case 0x02: // LED Base Camera PWM set
                        led1_duty_cycle = ReceivedDataBuffer[2];
                        Write_b_eep(baseLED_EEPROM_address, led1_duty_cycle);
                        led1_duty_cycle = led1_duty_cycle * 4;
                        SetDCPWM1(led1_duty_cycle);
                        break;

                    case 0x03: // LED Head Camera on/off
                        if (ReceivedDataBuffer[2] == 0x01){
                        OpenPWM2(0xFF);
                        led2_duty_cycle = led2_duty_cycle * 4;
                        SetDCPWM2(led2_duty_cycle);
                        //    outBaseLED = 1;
                        led2running = 1;
                        }else{
                           ClosePWM2();
                           // outBaseLED = 0;
                           led2running = 0;
                        }
                        break;

                    case 0x04: // LED Head Camera PWM set
                         led2_duty_cycle = ReceivedDataBuffer[2];
                         Write_b_eep(headLED_EEPROM_address, led2_duty_cycle);
                         led2_duty_cycle = led2_duty_cycle * 4;
                        SetDCPWM2(led2_duty_cycle);

                        break;
                    case 0x05: // LED Base Status
                        if (led1running == 1){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                           ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }

                        break;
                    case 0x06: // LED Head Status
                        if (led2running == 1){
                            ToSendDataBuffer[0] = 0x01;
                        }
                        else{
                           ToSendDataBuffer[0] = 0x00;
                        }
                        // Transmit the response to the host
                        if (!HIDTxHandleBusy(USBInHandle)) {
                            USBInHandle = HIDTxPacket(HID_EP, (BYTE*) & ToSendDataBuffer[0], 64);
                        }


                        break;

                    default: // Unknown command received
                        break;
                }
                break;



            default: // Unknown command received
                break;
        }
        
        // Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket(HID_EP, (BYTE*) & ReceivedDataBuffer, 64);
    }
}

// USB Callback handling routines -----------------------------------------------------------

// Call back that is invoked when a USB suspend is detected
void USBCBSuspend(void)
{
}

// This call back is invoked when a wakeup from USB suspend is detected.
void USBCBWakeFromSuspend(void)
{
}

// The USB host sends out a SOF packet to full-speed devices every 1 ms.
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here. Callback caller is already doing that.
}

// The purpose of this callback is mainly for debugging during development.
// Check UEIR to see which error causes the interrupt.
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.
}

// Check other requests callback
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}

// Callback function is called when a SETUP, bRequest: SET_DESCRIPTOR request arrives.
void USBCBStdSetDscHandler(void)
{
    // You must claim session ownership if supporting this request
}

//This function is called when the device becomes initialized
void USBCBInitEP(void)
{
    // Enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    
    // Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
}

// Send resume call-back
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    // Verify that the host has armed us to perform remote wakeup.
    if(USBGetRemoteWakeupStatus() == FLAG_TRUE) 
    {
        // Verify that the USB bus is suspended (before we send remote wakeup signalling).
        if(USBIsBusSuspended() == FLAG_TRUE)
        {
            USBMaskInterrupts();
            
            // Bring the clock speed up to normal running state
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FLAG_FALSE;

            // Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            // device must continuously see 5ms+ of idle on the bus, before it sends
            // remote wakeup signalling.  One way to be certain that this parameter
            // gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            // least 3ms from bus idle to USBIsBusSuspended() == FLAG_TRUE, yeilds
            // 5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            } while(delay_count);
            
            // Start RESUME signaling for 1-13 ms
            USBResumeControl = 1;
            delay_count = 1800U;
            do
            {
                delay_count--;
            } while(delay_count);
            USBResumeControl = 0;

            USBUnmaskInterrupts();
        }
    }
}

// USB callback function handler
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            // Application callback tasks and functions go here
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        default:
            break;
    }      
    return FLAG_TRUE; 
}

#endif
