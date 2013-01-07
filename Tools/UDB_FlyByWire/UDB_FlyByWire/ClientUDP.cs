using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace UDB_FlyByWire
{
    class ClientUDP
    {
        private Socket clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp); //The main client socket
        private MainForm my_owner;
        private bool m_IsConnected = false;

        public ClientUDP(MainForm owner)
        {
            my_owner = owner;
        }
        ~ClientUDP()
        {
            try
            {
                clientSocket.Close();
            }
            catch { }
        }
        public void Connect(string ip, ushort port)
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
