using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace LogoCmdSender
{
    class ClientTCP
    {
        private MainForm my_owner;
        TcpClient server = new TcpClient();
        private bool weWantToBeConnected = false;
        string m_ip = "";
        int m_port = 0;

        public ClientTCP(MainForm owner)
        {
            my_owner = owner;
        }

        public void Connect(string ip, ushort port)
        {
            m_ip = ip;
            m_port = port;

            weWantToBeConnected = true;
            Thread serverThread = new Thread(new ThreadStart(ServerThread));
            serverThread.Start();
        }

        public void Disconnect()
        {
            try
            {
                weWantToBeConnected = false;
                if (server.Connected)
                    server.Close();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientTCP Disconnect:\r\n" + ex.ToString());
            }
        }

        public bool Send(byte[] buffer)
        {
            bool success = false;
            try
            {
                if ((server != null) && server.Connected)
                {
                    NetworkStream stream = server.GetStream();
                    stream.Write(buffer, 0, buffer.Length);
                    stream.Flush();
                    success = true;
                }
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ClientTCP Send:\r\n" + ex.ToString());
            }
            return success;
        }
       
        private void ServerThread()
        {
            try
            {
                while (weWantToBeConnected)
                {
                    if (!server.Connected)
                        server = new TcpClient(m_ip, m_port);
                    byte[] message = new byte[4096];
                    int rxCount = server.Client.Receive(message);
                    my_owner.ParseRxPacket(message, rxCount);
                }
            }
            catch //(SystemException ex)
            {
                //my_owner.debug.Append("\r\nCrash in ServerThread:\r\n" + ex.ToString());
            }
            Disconnect();
        }

        public bool isConnected()
        {
            bool status = false;
            try
            {
                status = server.Connected;
            }
            catch
            {
                server = new TcpClient(m_ip, m_port);
            }
            return status;
        }


    } // class
} // namespace
