/************************************************************************
	debug.c

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

#ifndef DEBUG_C
#define DEBUG_C

// Global includes
#include <string.h>

// Local includes
#include "HardwareProfile.h"
#include "debug.h"

// Microchip Application Library includes
// (expects V2.9a of the USB library from "Microchip Solutions v2011-07-14")
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Only compile in the global debug variables if debugging is required
#if defined(DEBUGON)

	// Buffer pointers
	UINT debugBufferStart;
	UINT debugBufferEnd;
	UINT debugBufferLevel;
	
	// The following array is the cyclic buffer
	UINT8 debugBuffer[DEBUGBUFFERSIZE];

#endif

// Initialise the debugging log functionality
void debugInitialise(void)
{
#if defined(DEBUGON)
	// Reset the buffer's pointers
	debugBufferStart = 0;
	debugBufferEnd = 0;
	debugBufferLevel = 0;
#endif
}

// Send debug text to the debug log
void debugOut(char* debugString)
{
#if defined(DEBUGON)
	UINT charNumber;
	
	// Is there space in the debug buffer?
	if (debugBufferLevel + strlen(debugString) >= DEBUGBUFFERSIZE - 2)
	{
		// Buffer does not have enough space... silently drop the debug string
	}
	else
	{	
		// Buffer is not full, write the bytes and update the end pointer
		for (charNumber = 0; charNumber < strlen(debugString); charNumber++)
		{
			debugBuffer[debugBufferEnd] = debugString[charNumber];
			debugBufferEnd = (debugBufferEnd + 1) % DEBUGBUFFERSIZE;
			
			// Increment the buffer level indicator
			debugBufferLevel++;
		}
		
		// Add a return and new line to the end of the string
		debugBuffer[debugBufferEnd] = '\r';
		debugBufferEnd = (debugBufferEnd + 1) % DEBUGBUFFERSIZE;
		debugBufferLevel++;
		debugBuffer[debugBufferEnd] = '\n';
		debugBufferEnd = (debugBufferEnd + 1) % DEBUGBUFFERSIZE;
		debugBufferLevel++;
	}
#endif
}

// Copy 63 bytes of the debug buffer to the USB send buffer
// The first byte is the number of characters transferred
void copyDebugToSendBuffer(BYTE* sendDataBuffer)
{
#if defined(DEBUGON)
	UINT bytesToSend = 0;
	UINT byteCounter;
	
	// Determine the number of bytes to send
	if (debugBufferLevel > 63) bytesToSend = 63;
		else bytesToSend = debugBufferLevel;
	
	// Place the number of sent bytes in byte[0] of the send buffer
	sendDataBuffer[0] = bytesToSend - 1;
	
	if (debugBufferLevel != 0)
	{
		for (byteCounter = 1; byteCounter < bytesToSend; byteCounter++)
		{
			// Send the next byte to the send buffer
			sendDataBuffer[byteCounter] = debugBuffer[debugBufferStart];
			
			// Update the cyclic buffer pointer
			debugBufferStart = (debugBufferStart + 1) % DEBUGBUFFERSIZE;
			
			// Decrement the buffer level indicator
			debugBufferLevel--;
		}
	}
#else
	// Ensure that we indicate there is nothing to send if the host
	// requests debug
	sendDataBuffer[0] = 0;
#endif
}	
	
#endif
