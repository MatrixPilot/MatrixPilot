using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

// http://stackoverflow.com/questions/9393814/c-sharp-udp-server-client-nat


namespace UDB_FlyByDatalink
{
    class ClientUDP
    {
        private MainForm my_owner;
        private bool m_weWantToBeConnected = false;
        private UdpClient m_server = null;
        private int m_port = 0;
        private string m_ip = "";

        public ClientUDP(MainForm owner)
        {
            my_owner = owner;
        }
        ~ClientUDP()
        {
            Disconnect();
        }

        public void Connect(string ip, ushort port)
        {
            try
            {
                //ip = (Dns.GetHostAddresses(ip))[0].ToString();
            }
            catch { }

            try
            {
                m_port = port;
                m_ip = ip;

                Thread receiveThread = new Thread(new ThreadStart(ClientThread));
                m_weWantToBeConnected = true;
                receiveThread.Start();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientUDP Connect:\r\n" + ex.ToString());
                Disconnect();
            }
        }

        private void ClientThread()
        {
            try
            {
                Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                client.ReceiveTimeout = 1000;
                IPEndPoint end = new IPEndPoint(IPAddress.Any, m_port);
                client.Bind(end);

                while (m_weWantToBeConnected)
                {
                    try
                    {
                        byte[] message = new byte[1024];
                        client.Receive(message, 0, message.Length, SocketFlags.None);
                        my_owner.ParseRxPacket(message, message.Length);
                    }
                    catch { }
                }
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientUDP ClientThread:\r\n" + ex.ToString());
            }
        }

        public void Disconnect()
        {
            try
            {
                m_weWantToBeConnected = false;
                if (m_server != null)
                    m_server.Close();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientUDP Disconnect:\r\n" + ex.ToString());
            }
        }

        public void Send(byte[] data)
        {
            if (!isConnected())
                return;
            try
            {
                UdpClient client = new UdpClient(m_ip, m_port);
                client.Send(data, data.Length);
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientUDP Send:\r\n" + ex.ToString());
                //Disconnect();
            }
        }

        public bool isConnected()
        {
            return m_weWantToBeConnected;
        }

    }
}
