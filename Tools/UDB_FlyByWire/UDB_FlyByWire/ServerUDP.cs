using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UDB_FlyByWire
{
    class ServerUDP
    {
        private MainForm my_owner;
        private bool m_IsConnected = false;

        public ServerUDP(MainForm owner)
        {
            my_owner = owner;
        }

        public void StartListening(ushort port)
        {
        }
        public void StopListening()
        {
        }
        public void Disconnect()
        {
        }
        public void Send(byte[] data)
        {
        }
        public bool isConnected()
        {
            return m_IsConnected;
        }

    }
}
