using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using System.Xml;

// Helpful URL: http://www.switchonthecode.com/tutorials/csharp-tutorial-simple-threaded-tcp-server
namespace UDB_FlyByDatalink
{
    class ServerTCP
    {
        private TcpClient m_client = new TcpClient();
        private MainForm my_owner;
        private bool weWantToBeConnected = false;
        int m_port = 0;

        public ServerTCP(MainForm owner)
        {
            my_owner = owner;
        }

        ~ServerTCP()
        {
            try
            {
                if (m_client.Connected)
                {
                    m_client.Client.Disconnect(true);
                }
                    m_client.Client.Close();
                    //m_client.Close();
                
            }
            catch { m_client = null; }
        }

        public bool isConnected()
        {
            try
            {
                if ((m_client != null) && (m_client.Client != null))
                    return m_client.Client.Connected;
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nError serverTCP isConnected\r\n + " + ex.ToString());
            }
            return false;
        }

        public void StartListening(ushort port)
        {
            try
            {
                //if (DynDNS_textBox.Text.Length > 0)
                //    my_owner.debug.Append("DynDNS IP: " + System.Net.Dns.GetHostAddresses(DynDNS_textBox.Text)[0].ToString() + "\r\n");
                //my_owner.debug.Append("Public IP: " + getPublicIP() + "\r\n");
                /*
                string hostname = Dns.GetHostName();
                IPHostEntry host = Dns.GetHostEntry(hostname);
                IPAddress[] iplist = host.AddressList;

                for (int i = 0; i < iplist.Length; i++)
                {
                    if (iplist[i].AddressFamily != AddressFamily.InterNetworkV6)
                        my_owner.debug.Append("Intern IP: " + Dns.GetHostEntry(Dns.GetHostName()).AddressList[i].ToString() + "\r\n");
                }
                my_owner.debug.Append("\r\n");
                 * */

                m_port = port;
                weWantToBeConnected = true;
                Thread listenThread = new Thread(new ThreadStart(ServerThread));
                listenThread.Start();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nError serverTCP StartListening\r\n + " + ex.ToString());
                StopListening();
            }
        }
        public void StopListening()
        {
            weWantToBeConnected = false;
            try 
            {
                if (isConnected())
                    m_client.Client.Disconnect(true);
                m_client.Client.Close();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nError serverTCP StopListening\r\n + " + ex.ToString());
            }
        }
       
        private void ServerThread()
        {
            try
            {
                NetworkStream stream = null;
                byte[] message = new byte[4096];
                int bytesRead;

                while (weWantToBeConnected)
                {
                    if ((m_client != null) && (m_client.Client != null) && m_client.Client.Connected)
                        m_client.Client.Disconnect(true);

                    m_client = new TcpClient();

                    TcpListener tcpListener = new TcpListener(IPAddress.Any, m_port);
                    tcpListener.Start();

                    while (weWantToBeConnected)
                    {
                        if (!m_client.Connected)
                        {
                            m_client = tcpListener.AcceptTcpClient(); // blocks until someone connects
                            stream = m_client.GetStream(); // configure socket
                        }
                        else
                        {
                            // Blocks until data is recieved, crashes when socket closes
                            bytesRead = stream.Read(message, 0, 4096);
                            my_owner.ParseRxPacket(message, bytesRead);
                        }
                    }
                }
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nError serverTCP ServerThread\r\n" + ex.ToString());
                //my_owner.debug.Append("x");
            }
            StopListening();
        }

        public void Send(byte[] buffer)
        {
            try
            {
                if (m_client.Client.Connected)
                {
                    NetworkStream stream = m_client.GetStream();
                    stream.Write(buffer, 0, buffer.Length);
                    stream.Flush();
                }
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nError serverTCP Send\r\n + " + ex.ToString());
            }
        }

    } // class
} // namespace
