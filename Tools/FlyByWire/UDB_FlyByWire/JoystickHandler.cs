using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX.DirectInput;

namespace UDB_FlyByWire
{
    class JoystickHandler
    {
        const string PacketHeader = "FbW";
        const int PacketLength = 13;
        public int CenterX = 0;
        public int CenterY = 0;
        public bool InvertX = false;
        public bool InvertY = false;
        public bool InvertRudder = false;
        public bool InvertThrottle = false;

        public class FbW_Data
        {
            public string header;
            public int aileron;
            public int elevator;
            public int rudder;
            public int throttle;
            public int mode;
            public bool hasChanged;

            public FbW_Data()
            {
                header = PacketHeader;
                aileron = 0;
                elevator = 0;
                rudder = 0;
                throttle = 0;
                mode = 0;
                hasChanged = true;
            }

            public FbW_Data(int _aileron, int _elevator, int _rudder, int _throttle, int _mode)
            {
                header = PacketHeader;
                aileron = _aileron;
                elevator = _elevator;
                rudder = _rudder;
                throttle = _throttle;
                mode = _mode;
                hasChanged = true;
            }

        }

        public int ConvertForUI(int value, decimal scalar, decimal trim, bool isInverted)
        {
            if (isInverted)
                value = MainForm.JoyMaxValue - value;

            // apply centering calibration and convert to +/- 32k
            value -= CenterX;

            // apply scaler
            value = Convert.ToInt32(value * scalar);

            // convert back to 0-65k
            value += (MainForm.JoyMidValue); // Mid = (min+max)/2

            // translate 0-65535 to 2000-4000
            value = MainForm.PWMminValue + (value * MainForm.PWMrange) / MainForm.JoyMaxValue;

            // add trim offset to end result
            value += Convert.ToInt32(trim);

            // I'm too lazy to optimize this, at least this way it's clear what the math is doing
            return value;
        }
        
        public void ParseRxPacket(byte[] packet)
        {
            // we dont care about Rx data
        }

        static public byte[] CreateTxPacket(FbW_Data PWMdata)
        {
            byte[] packet = new byte[PacketLength];
            int i = 0;

            packet[i++] = (byte)PWMdata.header[0];
            packet[i++] = (byte)PWMdata.header[1];
            packet[i++] = (byte)PWMdata.header[2];

            packet[i++] = (byte)(PWMdata.aileron); // LSB first
            packet[i++] = (byte)(PWMdata.aileron >> 8);
            packet[i++] = (byte)(PWMdata.elevator);
            packet[i++] = (byte)(PWMdata.elevator >> 8);
            packet[i++] = (byte)(PWMdata.mode);
            packet[i++] = (byte)(PWMdata.mode >> 8);
            packet[i++] = (byte)(PWMdata.rudder);
            packet[i++] = (byte)(PWMdata.rudder >> 8);
            packet[i++] = (byte)(PWMdata.throttle);
            packet[i++] = (byte)(PWMdata.throttle >> 8);

            return packet;
        }

        static public byte CreateChecksum(byte[] packet)
        {
            byte checksum = 0;

            for (int i = 0; i < packet.Length; i++)
                checksum += packet[i];
            return checksum;
        }



    }
}
