using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

// http://stackoverflow.com/questions/9393814/c-sharp-udp-server-client-nat

namespace UDB_FlyByWire
{
    class ServerUDP
    {
        private MainForm my_owner;
        private bool m_weWantToBeConnected = false;
        private UdpClient m_client = null;
        IPEndPoint m_serverEP = new IPEndPoint(IPAddress.Any, 0);

        public ServerUDP(MainForm owner)
        {
            my_owner = owner;
        }
        ~ServerUDP()
        {
            StopListening();
        }

        public void StartListening(ushort port)
        {
            try
            {
                if (m_client != null)
                    m_client.Close();

                m_client = new UdpClient(port);
                m_weWantToBeConnected = true;
                Thread receiveThread = new Thread(new ThreadStart(ServerThread));
                receiveThread.Start();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ServerUDP StartListening:\r\n" + ex.ToString());
            }
        }

        private void ServerThread()
        {
            while (m_weWantToBeConnected && (m_client != null))
            {
                try
                {
                    // once someone connects to us m_anyIP populates with their IP and port.
                    byte[] message = m_client.Receive(ref m_serverEP);
                    my_owner.ParseRxPacket(message, message.Length);
                }
                catch //(SystemException ex)
                {
                    //my_owner.debug.Append("\r\nCrash in ServerUDP ServerThread:\r\n" + ex.ToString());
                }
            }
            m_weWantToBeConnected = false;
        }

        public void StopListening()
        {
            try
            {
                m_weWantToBeConnected = false;
                if (m_client != null)
                    m_client.Close();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ServerUDP StopListening:\r\n" + ex.ToString());
            }
        }

        public void Send(byte[] data)
        {
            try
            {
                if (m_weWantToBeConnected && (m_client != null) && (m_serverEP.Address != IPAddress.Any))
                {
                    //IPEndPoint remoteEndPoint = new IPEndPoint(m_serverEP.Address, m_serverEP.Port);
                    //m_client.Send(data, data.Length, remoteEndPoint);

                    m_client.Send(data, data.Length, m_serverEP);
                }
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ServerUDP Send:\r\n" + ex.ToString());
                StopListening();
            }
        }

        public bool isConnected()
        {
            return m_weWantToBeConnected;
        }

    }
}
