using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace PNPControllerUSB
{
    using usbGenericHidCommunications;
    class usbDevice : usbGenericHidCommunication
    {
        /// <summary>
        /// Class constructor - place any initialisation here
        /// </summary>
        /// <param name="vid"></param>
        /// <param name="pid"></param>
        public usbDevice(int vid, int pid)
            : base(vid, pid)
        {
        }


        public bool getDeviceStatus()
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x01;
            outputBuffer[2] = 0x01;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            return success;
        }

        public bool getFeederReadyStatus()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x02;
            outputBuffer[2] = 0x02;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 0) return true;
                if (inputBuffer[1] == 1) return false;
            }

            return success;
        }

        public bool setGotoFeeder(byte inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x02;
            outputBuffer[2] = 0x03;
            outputBuffer[3] = inval;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setResetFeederZ()
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x02;
            outputBuffer[2] = 0x04;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setResetFeeder()
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x02;
            outputBuffer[2] = 0x05;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setResetArduino()
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x02;
            outputBuffer[2] = 0x06;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setVAC1(bool inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x01;
            if (inval)
            {
                outputBuffer[3] = 0x01;
            }
            else
            {
                outputBuffer[3] = 0x00;
            }

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setVAC2(bool inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x02;
            if (inval)
            {
                outputBuffer[3] = 0x01;
            }
            else
            {
                outputBuffer[3] = 0x00;
            }

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setVibrationMotor(bool inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x03;
            if (inval)
            {
                outputBuffer[3] = 0x01;
            }
            else
            {
                outputBuffer[3] = 0x00;
            }

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setVibrationMotorSpeed(byte inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x07;
            outputBuffer[3] = inval;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool getVac1Status()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x04;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 1) return true;
                if (inputBuffer[1] == 0) return false;
            }

            return success;
        }

        public bool getVac2Status()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x05;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 1) return true;
                if (inputBuffer[1] == 0) return false;
            }

            return success;
        }

        public bool getVibrationStatus()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x03;
            outputBuffer[2] = 0x06;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 1) return true;
                if (inputBuffer[1] == 0) return false;
            }

            return success;
        }

        public bool setBaseCameraLED(bool inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x01;
            if (inval)
            {
                outputBuffer[3] = 0x01;
            }
            else
            {
                outputBuffer[3] = 0x00;
            }

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setHeadCameraLED(bool inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x03;
            if (inval)
            {
                outputBuffer[3] = 0x01;
            }
            else
            {
                outputBuffer[3] = 0x00;
            }

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setBaseCameraPWM(byte inval)
        {
            Byte[] outputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x02;
            outputBuffer[3] = inval;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool setHeadCameraPWM(byte inval)
        {
            Byte[] outputBuffer = new Byte[65];


            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x04;
            outputBuffer[3] = inval;

            bool success;
            success = writeRawReportToDevice(outputBuffer);
            return success;
        }

        public bool getBaseCameraLEDStatus()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x05;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 1) return true;
                if (inputBuffer[1] == 0) return false;
            }

            return success;
        }

        public bool getHeadCameraLEDStatus()
        {
            Byte[] outputBuffer = new Byte[65];
            Byte[] inputBuffer = new Byte[65];

            outputBuffer[0] = 0;
            outputBuffer[1] = 0x04;
            outputBuffer[2] = 0x06;

            bool success;
            success = writeRawReportToDevice(outputBuffer);

            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
                if (inputBuffer[1] == 1) return true;
                if (inputBuffer[1] == 0) return false;
            }

            return success;
        }







        // Collect debug information from the device
        public String collectDebug()
        {
            // Collect debug information from USB device
            Debug.WriteLine("Reference Application -> Collecting debug information from device");

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x10;

            // Send the collect debug command
            writeRawReportToDevice(outputBuffer);

            // Read the response from the device
            readSingleReportFromDevice(ref inputBuffer);

            // Byte 1 contains the number of characters transfered
            if (inputBuffer[1] == 0) return String.Empty;

            // Convert the Byte array into a string of the correct length
            string s = System.Text.ASCIIEncoding.ASCII.GetString(inputBuffer, 2, inputBuffer[1]);

            return s;
        }
    }
}