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
namespace UDB_FlyByWire
{
    class ServerTCP
    {
        private TcpListener tcpListener;
        private Thread listenThread;
        private TcpClient client = null;
        private NetworkStream clientStream = null;

        private MainForm my_owner;
        private bool weWantToBeConnected = false;

        public ServerTCP(MainForm owner)
        {
            my_owner = owner;
        }

        ~ServerTCP()
        {
            try
            {
                try
                {
                    if (client != null)
                        client.Close();
                    if (tcpListener != null)
                        tcpListener.Stop();
                    if (listenThread != null)
                        listenThread.Abort();
                }
                catch { }

            }
            catch { }
        }

        public bool isConnected()
        {
            if (client != null)
                return client.Connected;
            else
                return false;
        }

        public void StartListening(ushort port)
        {
            try
            {
                weWantToBeConnected = true;
                my_owner.debug.Append("Starting TCP server\r\n");
                //if (DynDNS_textBox.Text.Length > 0)
                //    my_owner.debug.Append("DynDNS IP: " + System.Net.Dns.GetHostAddresses(DynDNS_textBox.Text)[0].ToString() + "\r\n");
                //my_owner.debug.Append("Public IP: " + getPublicIP() + "\r\n");

                string hostname = Dns.GetHostName();
                IPHostEntry host = Dns.GetHostEntry(hostname);
                IPAddress[] iplist = host.AddressList;

                for (int i = 0; i < iplist.Length; i++)
                {
                    if (iplist[i].AddressFamily != AddressFamily.InterNetworkV6)
                        my_owner.debug.Append("Intern IP: " + Dns.GetHostEntry(Dns.GetHostName()).AddressList[i].ToString() + "\r\n");
                }
                my_owner.debug.Append("\r\n");
                this.tcpListener = new TcpListener(IPAddress.Any, port);
                this.listenThread = new Thread(new ThreadStart(ListenForClients));
                this.listenThread.Start();
            }
            catch
            {
                my_owner.debug.Append("Error Starting Server");
            }
        }
        public void StopListening()
        {
            weWantToBeConnected = false;

            if ((client != null) && client.Connected)
                client.Close();
            
            if (tcpListener != null)
                tcpListener.Stop();
            if (listenThread != null)
                listenThread.Abort();
            
            System.Threading.Thread.Sleep(100);

            this.tcpListener = null;
            this.listenThread = null;
            
        }
       
        private void HandleClientComm(object new_client)
        {
            client = (TcpClient)new_client;
            clientStream = client.GetStream();
            bool valid_bytesRead = false;

            try
            {
                int bytesRead;
                byte[] message = new byte[4096];
                ASCIIEncoding encoder = new ASCIIEncoding();


                if (weWantToBeConnected == false)
                    client.Close();

                while ((my_owner.IsClosing == false) && weWantToBeConnected)
                {
                    bytesRead = 0;
                    valid_bytesRead = false;
                    try
                    {
                        while ((my_owner.IsClosing == false) && (valid_bytesRead == false) && (client.Connected == true))
                        {
                            //blocks until a client sends a message
                            bytesRead = clientStream.Read(message, 0, 4096);
                            valid_bytesRead = true;
                            if (bytesRead == 0)
                            {
                                break;
                            }
                        } // while
                    }
                    catch
                    {
                        break;
                    }

                    if (bytesRead == 0)
                    {
                        break; //the client has disconnected from the server
                    }
                    else if (valid_bytesRead == true)
                    {
                        //message has successfully been received.
                        my_owner.ParseRxPacket(message, bytesRead);
                    }
                    if (weWantToBeConnected == false)
                        client.Close();
                } // while

            }
            catch //(SystemException ex)
            {
            }
            if (client != null)
                client.Close();
        }

        private void ListenForClients()
        {
            try
            {
                this.tcpListener.Start();

                while (!my_owner.IsClosing && (tcpListener != null))
                {
                    //blocks until a client has connected to the server
                    TcpClient client = tcpListener.AcceptTcpClient();

                    //create a thread to handle communication 
                    //with connected client
                    Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));

                    // returns when thread closes (client disconnects)
                    clientThread.Start(client);
                } // while
                if (tcpListener != null)
                    tcpListener.Stop();
            }
            catch
            {
                try
                {
                    if (tcpListener != null)
                        tcpListener.Stop();
                }
                catch { }
            }
        }

        public void Send(byte[] buffer)
        {
            if ((client != null) && client.Connected)
            {
                NetworkStream stream = client.GetStream();
                stream.Write(buffer, 0, buffer.Length);
                stream.Flush();
            }
        }

    } // class
} // namespace
