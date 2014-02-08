/************************************************************************
	HardwareProfile.h

	WFF USB Generic HID Demonstration 3
    usbGenericHidCommunication reference firmware 3_0_0_0
    Copyright (C) 2011 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com

************************************************************************/

#ifndef HARDWAREPROFILE_H
#define HARDWAREPROFILE_H

// USB stack hardware selection options ----------------------------------------------------------------

// (This section is the set of definitions required by the MCHPFSUSB framework.)

// Uncomment the following define if you wish to use the self-power sense feature 
// and define the port, pin and tris for the power sense pin below:
// #define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2
#if defined(USE_SELF_POWER_SENSE_IO)
	#define self_power          PORTAbits.RA2
#else
	#define self_power          1
#endif

// Uncomment the following define if you wish to use the bus-power sense feature 
// and define the port, pin and tris for the power sense pin below:
//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISAbits.TRISA1
#if defined(USE_USB_BUS_SENSE_IO)
	#define USB_BUS_SENSE       PORTAbits.RA1
#else
	#define USB_BUS_SENSE       1
#endif

// Uncomment the following line to make the output HEX of this project work with the MCHPUSB Bootloader    
//#define PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER

// Uncomment the following line to make the output HEX of this project work with the HID Bootloader
#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER		

// Application specific hardware definitions ------------------------------------------------------------

// Oscillator frequency (48Mhz with a 20Mhz external oscillator)
#define CLOCK_FREQ 48000000

// Device Vendor Indentifier (VID) (0x04D8 is Microchip's VID)
#define USB_VID	0x04D8

// Device Product Indentifier (PID) (0x0042)
#define USB_PID	0x0042

// Manufacturer string descriptor
#define MSDLENGTH	17
#define MSD		'A','B',' ','E','l','e','c','t','r','o','n','i','c','s',' ','U','K'

// Product String descriptor
#define PSDLENGTH	25
#define PSD		'P','i','c','k',' ','a','n','d',' ','P','l','a','c','e',' ','C','o','n','t','r','o','l','l','e','r'

// Device serial number string descriptor
#define DSNLENGTH	7
#define DSN		'V','e','r','_','3','.','0'

// Common useful definitions
#define INPUT_PIN 1
#define OUTPUT_PIN 0
#define FLAG_FALSE 0
#define FLAG_TRUE 1

// Comment out the following line if you do not want the debug
// feature of the firmware (saves code and RAM space when off)
//
// Note: if you use this feature you must compile with the large
// memory model on (for 24-bit pointers) so that the sprintf()
// function will work correctly.  If you do not require debug it's
// recommended that you compile with the small memory model and 
// remove any references to <strings.h> and sprintf().
//#define DEBUGON

// PIC to hardware pin mapping and control macros


#define sdaPin PORTBbits.RB0
#define sclPin PORTBbits.RB1
#define atmegaResetPin PORTBbits.RB2
#define atmegaFeederRunning PORTBbits.RB3
#define atmegaFeederRunningTRIS TRISBbits.RB3

#define outBaseLED PORTCbits.RC2
#define outHeadLED PORTCbits.RC1

#define setVac1on PORTDbits.RD0 = 1; PORTDbits.RD1 = 0; vac1running = 1;
#define setVac1off PORTDbits.RD0 = 0; PORTDbits.RD1 = 1; vac1running = 0;

#define setVac2on PORTDbits.RD2 = 1; PORTDbits.RD3 = 0; vac2running = 1;
#define setVac2off PORTDbits.RD2 = 0; PORTDbits.RD3 = 1; vac2running = 0;

#define setVibrationon PORTAbits.RA4 = 1; vibrationrunning = 1;
#define setVibrationoff PORTAbits.RA4 = 0; vibrationrunning = 0;






// IO Pins






#endif
