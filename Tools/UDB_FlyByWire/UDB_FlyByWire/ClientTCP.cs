using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace UDB_FlyByWire
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
            Thread serverThreadnew = new Thread(new ThreadStart(ServerThread));

            m_ip = ip;
            m_port = port;

            weWantToBeConnected = true;
            serverThreadnew.Start();
        }

        public void Disconnect()
        {
            weWantToBeConnected = false;
            if (server.Connected)
                server.Close();
        }

        public void Send(byte[] buffer)
        {
            if ((server != null) && server.Connected)
            {
                NetworkStream stream = server.GetStream();
                stream.Write(buffer, 0, buffer.Length);
                stream.Flush();
            }
        }
       
        private void ServerThread()
        {
            try
            {
                if (server.Connected)
                    server.Close();

                while (weWantToBeConnected)
                {
                    if (!server.Connected)
                        server = new TcpClient(m_ip, m_port);
                    System.Threading.Thread.Sleep(1000);
                }
                if (server.Connected)
                    server.Close();
            }
            catch (SystemException ex)
            {
                my_owner.debug.Append("\r\nCrash in ServerThread:\r\n" + ex.ToString());
            }
        }

        public bool isConnected()
        {
            return server.Connected;
        }


    } // class
} // namespace
