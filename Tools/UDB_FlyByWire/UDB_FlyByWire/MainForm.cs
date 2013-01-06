using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.Diagnostics;
using Microsoft.DirectX.DirectInput;
using System.IO;
using System.Xml;


namespace UDB_FlyByWire
{
    public partial class Form1 : Form
    {
        private TcpListener tcpListener;
        private Thread listenThread;
        public string data = "";
        public bool IsClosing = false;
        public bool ClientIsConnected = false;

        private UDB_FlyByWire.Joystick jyst;
        private DeviceList gameControllerList;
    
        public int Joy_centerX=0;
        public int Joy_centerY=0;
        public bool Joy_Values_Are_New = true;
        public byte[] Joymessage = new byte[10+3];
        public int Joy_index = 0;

        public int bytesReceieved = 0;

        public Form1()
        {
            InitializeComponent();

            //this.listenThread.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Text += "  v" + Application.ProductVersion;
            
            this.tcpListener = new TcpListener(IPAddress.Any, Convert.ToInt16(Port_textBox.Text));
            this.listenThread = new Thread(new ThreadStart(ListenForClients));

            port_label.Text = "Port: " + Port_textBox.Text;
            Set_Joystick_Settings(null, null);          /* Find joystick if there is one and list in Menu -> Settings -> Joystick */
            Mode_comboBox.SelectedIndex = 0;

            try
            {
                gameControllerList = Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.AttachedOnly);
                if (gameControllerList.Count > 0)
                {
                    jyst.Poll();
                    Joy_centerX = jyst.state.X;
                    Joy_centerY = jyst.state.Y;
                }
                else
                {
                    textBox1.Text += "No Joystick found\r\n";
                    joystickComboBox.Items.Clear();
                    joystickComboBox.SelectedIndex = -1;
                }
            }
            catch { }


        }


        public void Set_Joystick_Settings(object sender, EventArgs e)
        {
            int i;
            string[] sticks;


            // grab the joystick
            jyst = new Joystick(this.Handle);
            sticks = jyst.FindJoysticks();
            joystickComboBox.Items.Clear();

            if (sticks == null)
            {
               IsJoyFound_label.Text = "No Joystick";
                return;
            }

            for (i = 0; i < sticks.Length; i++)
            {
                joystickComboBox.Items.Add(sticks[i]);
            }
            try
            {
                joystickComboBox.SelectedIndex = 0;
            }
            catch
            {
                joystickComboBox.Items.Clear();
                joystickComboBox.Items.Add("None");
                joystickComboBox.SelectedIndex = 0;
                IsJoyFound_label.Text = "No Joystick";
                textBox1.Text += "Failed assigning Joystick\r\n";
            }

            Get_Joystick(null, null);

        }

        public void Get_Joystick(object sender, EventArgs e)
        {
            //if (ignore_joystick_checkbox.Checked == true) return;
            //if ((joystickComboBox.SelectedIndex == -1) || (IsJoyFound_label.Text.Contains("No") == true)) return;

            try
            {
                jyst.AcquireJoystick(joystickComboBox.Items[joystickComboBox.SelectedIndex].ToString());
                IsJoyFound_label.Text = "Joystick Found";
            }
            catch
            {
                IsJoyFound_label.Text = "No Joystick";
            }


        }


        private void ListenForClients()
        {
            try
            {
                ClientIsConnected = false;
                this.tcpListener.Start();
                ClientIsConnected = false;

                while (!IsClosing)
                {
                    ClientIsConnected = false;
                    //blocks until a client has connected to the server
                    TcpClient client = tcpListener.AcceptTcpClient();

                    //create a thread to handle communication 
                    //with connected client
                    ClientIsConnected = false;

                    Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));
                    ClientIsConnected = false;

                    
                    //data += tcpListener.LocalEndpoint.ToString();
                    //data += GetIPAddress();
                    clientThread.Start(client);
                    ClientIsConnected = false;
                }
                ClientIsConnected = false;
                tcpListener.Stop();


            }
            catch
            {
                try
                {
                    ClientIsConnected = false;
                    tcpListener.Stop();
                }
                catch { }
            }
        }



        private void HandleClientComm(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream clientStream = tcpClient.GetStream();
            bool valid_bytesRead = false;

            try
            {
                int bytesRead;
                byte[] message = new byte[4096];
                ASCIIEncoding encoder = new ASCIIEncoding();


                if (EnableTCPServer_checkBox.Checked == false)
                    tcpClient.Close();

                while ((IsClosing == false) && EnableTCPServer_checkBox.Checked)
                {
                    bytesRead = 0;
                    valid_bytesRead = false;
                    try
                    {
                        try
                        {
                            while ((IsClosing == false) && (valid_bytesRead == false) && (tcpClient.Connected == true))
                            {
                                //blocks until a client sends a message
                                bytesRead = clientStream.Read(message, 0, 4096);
                                bytesReceieved += bytesRead;
                                valid_bytesRead = true;
                                if ((bytesRead == 0)) //&& (tcpClient.Connected == true))
                                {
                                    ClientIsConnected = false;
                                    break;
                                }
                            }
                        }
                        catch
                        {
                            ClientIsConnected = false;
                            //tcpListener.Stop();
                            break;
                        }
                    }
                    catch
                    {
                        ClientIsConnected = false;
                        //a socket error has occured
                        break;
                    }

                    if (bytesRead == 0)
                    {
                        ClientIsConnected = false;
                        break; //the client has disconnected from the server
                    }
                    else if (valid_bytesRead == true)
                    {
                        ClientIsConnected = true;

                        //message has successfully been received. show  in textbox via timer
                        if (TCPDebug_checkBox.Checked)
                            data = encoder.GetString(message, 0, bytesRead);


                        
                        if (Joy_Values_Are_New == true)
                        {
                            Joy_Values_Are_New = false;

                            if (JoyIP_checkBox.Checked == true)
                            {
                                clientStream.Write(Joymessage, 0, Joy_index); // Joystick data to Plane
                                clientStream.Flush();
                            }
                        }
                         
                    }

                }
                try
                {
                    ClientIsConnected = false;
                    //tcpListener.Stop();
                }
                catch { }
                ClientIsConnected = false;

            }
            catch //(SystemException ex)
            {

                try
                {
                     tcpClient.Close();
                    //listenThread.Abort();
                    //tcpListener.Stop();
                }
                catch { }
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IsClosing = true;

            try
            {
                jyst.ReleaseJoystick(); // Releases the current Joystick.
            }
            catch { }

            try
            {
                //listenThread.Abort();
                tcpListener.Stop();
                listenThread.Abort();
            }
            catch { }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (data.Length > 0)
            {
                textBox1.Text += data;
                data = "";
            }
        }

        private void timer_status_Tick(object sender, EventArgs e)
        {
            radioButton1.Checked = ClientIsConnected;
        }

        private void Joystick_timer_Tick(object sender, EventArgs e)
        {
            int x=0, y=0, z = 0, temp=0;

            try
            {
                if (OverrideJoy_checkBox.Checked)
                {
                    x = Aileron_trackBar.Value;
                    y = Elevator_trackBar.Value;
                    //rudder = Rudder_trackBar.Value;
                    z = Throttle_trackBar.Value;
                }
                else if (joystickComboBox.SelectedIndex >= 0)
                {
                    jyst.Poll();
                    // values are 0 to 65525
                    x = jyst.state.X;
                    y = jyst.state.Y;
                    //z = jyst.state.Z;
                    z = jyst.state.Z;

                    // offset to +-/ 32k
                    x -= Joy_centerX;
                    y -= Joy_centerY;
                    //z -= Joy_centerZ; // not needed

                    // translate to +/- 100%
                    if (x != 0)
                        x = (x * 100) / (65535 / 2);

                    if (y != 0)
                        y = (y * 100) / (65535 / 2);

                    if (z != 0)
                        z = (z * 100) / (65535 / 2);
                }

                if (InvertX_checkBox.Checked)
                    x = -x;
                if (InvertY_checkBox.Checked)
                    y = -y;
                if (InvertZ_checkBox.Checked)
                    z = -z;

                if (OverrideJoy_checkBox.Checked == false)
                {
                    if (x > 100)
                        x = 100;
                    else if (x < -100)
                        x = -100;

                    if (y > 100)
                        y = 100;
                    else if (y < -100)
                        y = -100;

                    if (z > 100)
                        z = 100;
                    else if (z < -100)
                        z = -100;

                    Aileron_trackBar.Value = x;
                    Elevator_trackBar.Value = y;
                    Throttle_trackBar.Value = z;
                }

                // UDB PWM values are 2200 to 3800
                // center is 3000 with +/- 800

                if (CopyXtoZ_checkBox.Checked)
                {
                    temp = x;
                    x = z;
                    z = temp;
                }
                if (CopyXtoY_checkBox.Checked)
                {
                    temp = x;
                    x = y;
                    y = temp;
                }
                if (CopyYtoZ_checkBox.Checked)
                {
                    temp = y;
                    y = z;
                    z = temp;
                }

                ushort aileron = (ushort)((x * 8) + 3000);
                ushort elevator = (ushort)((y * 8) + 3000);
                ushort throttle = (ushort)((z * 8) + 3000);
                ushort rudder = (ushort)0;

                ushort mode = 0;
                switch (Mode_comboBox.SelectedIndex)
                {
                    default:
                    case 2400: mode = 2400; break; // Manual
                    case 3000: mode = 2400; break; // Stabilized
                    case 3600: mode = 2400; break; // WayPoint
                }

                Load_JoyMessage(aileron, elevator, mode, rudder, throttle);

                if (JoyStickDebug_checkBox.Checked)
                {
                    textBox1.Text = "";
                    textBox1.Text += "x = " + x.ToString() + "\r\n";
                    textBox1.Text += "y = " + y.ToString() + "\r\n";
                    textBox1.Text += "z = " + z.ToString() + "\r\n";
                    textBox1.Text += "ailerons = " + aileron.ToString() + "\r\n";
                    textBox1.Text += "elevator = " + elevator.ToString() + "\r\n";
                    textBox1.Text += "throttle = " + throttle.ToString() + "\r\n";
                    textBox1.Text += "rudder   = " + rudder.ToString() + "\r\n";
                }

            }
            catch //(SystemException ex)
            {
                //JoyStickDebug_checkBox.Checked = false;
                textBox1.Text += "Crashed somewhere in Joy timer\r\n";

                //textBox1.Text += ex.ToString();
                //NudgeLabel.Text = "Error";
                //return;
            }

        }

        private void EnableServer_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (EnableTCPServer_checkBox.Checked)
                {
                    textBox1.Text += "Starting TCP server\r\n";
                    if (DynDNS_textBox.Text.Length > 0)
                        textBox1.Text += "DynDNS IP: " + System.Net.Dns.GetHostAddresses(DynDNS_textBox.Text)[0].ToString() + "\r\n";
                    textBox1.Text += "Public IP: " + getPublicIP() + "\r\n";

                    string hostname = Dns.GetHostName();
                    IPHostEntry host = Dns.GetHostEntry(hostname);
                    IPAddress[] iplist = host.AddressList;

                    for (int i = 0; i < iplist.Length; i++)
                    {
                        if (iplist[i].AddressFamily != AddressFamily.InterNetworkV6)
                            textBox1.Text += "Intern IP: " + Dns.GetHostEntry(Dns.GetHostName()).AddressList[i].ToString() + "\r\n";
                    }
                    textBox1.Text += "\r\n";
                    this.tcpListener = new TcpListener(IPAddress.Any, Convert.ToInt16(Port_textBox.Text));
                    this.listenThread = new Thread(new ThreadStart(ListenForClients));
                    this.listenThread.Start();
                }
                else
                {
                    this.tcpListener.Stop();
                    this.listenThread.Abort();
                    System.Threading.Thread.Sleep(100);
                    this.tcpListener = null;
                    this.listenThread = null;
                }
            }
            catch
            {
                textBox1.Text = "Error Starting Server";
            }
        }


        public void Load_JoyMessage(ushort ailerons, ushort elevator, ushort mode, ushort rudder, ushort throttle)
        {
            Joy_index = 0;
            
            Joymessage[Joy_index++] = (byte)('F');
            Joymessage[Joy_index++] = (byte)('b');
            Joymessage[Joy_index++] = (byte)('W');
            Joymessage[Joy_index++] = (byte)(ailerons); // LSB first
            Joymessage[Joy_index++] = (byte)(ailerons >> 8);
            Joymessage[Joy_index++] = (byte)(elevator);
            Joymessage[Joy_index++] = (byte)(elevator >> 8);
            Joymessage[Joy_index++] = (byte)(mode);
            Joymessage[Joy_index++] = (byte)(mode >> 8);
            Joymessage[Joy_index++] = (byte)(rudder);
            Joymessage[Joy_index++] = (byte)(rudder >> 8);
            Joymessage[Joy_index++] = (byte)(throttle);
            Joymessage[Joy_index++] = (byte)(throttle >> 8);

            Joy_Values_Are_New = true;
        }
        public string getPublicIP()
        {
            string direction;
            WebRequest request = WebRequest.Create("http://checkip.dyndns.org/");
            WebResponse response = request.GetResponse();
            StreamReader stream = new StreamReader(response.GetResponseStream());
            direction = stream.ReadToEnd();
            stream.Close();
            response.Close();

            //Search for the ip in the html
            int first = direction.IndexOf("Address: ") + 9;
            int last = direction.LastIndexOf("</body>");
            direction = direction.Substring(first, last - first);

            return direction;
        }

        private void JoyReset_button_Click(object sender, EventArgs e)
        {
            Aileron_trackBar.Value = 0;
            Elevator_trackBar.Value = 0;
            Rudder_trackBar.Value = 0;
        }

        private void TextBoxClear_button_Click(object sender, EventArgs e)
        {
            textBox1.Text = "";
        }

        private void HouseKeeping_1sec_timer_Tick(object sender, EventArgs e)
        {
            int bytesRx = bytesReceieved;
            bytesReceieved = 0;
            DataRx_label.Text = "Rx Rate: " + (1000 * bytesRx / HouseKeeping_1sec_timer.Interval).ToString() + " B/s";
        }

        private void EmailTomPittenger_button_Click(object sender, EventArgs e)
        {
            string email;
            email = "mailto:magicrub@gmail.com?subject=UAV Devboard FlyByWire&body=";
            email += "Hi Tom!";

            try
            {
                System.Diagnostics.Process.Start(email);
            }
            catch (SystemException)
            {
                MessageBox.Show("Error opening your Email Program.\nSend email to: magicrub@gmail.com", "Email Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        

    }
}
