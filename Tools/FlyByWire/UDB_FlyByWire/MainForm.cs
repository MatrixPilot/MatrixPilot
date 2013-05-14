using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.DirectX.DirectInput;
using System.IO;
using System.IO.Ports;

namespace UDB_FlyByWire
{
    public partial class MainForm : Form
    {
        public const uint RX_TIMEOUT_RESET = 3; // # of seconds until we are disconnected in UDP

        //public string data = "";
        public bool IsClosing = false;

        private UDB_FlyByWire.JoystickMngr jyst;
        private JoystickHandler jystHandler = new JoystickHandler();

        public StringBuilder debug = new StringBuilder("");
        private ClientTCP clientTCP = null;
        private ClientUDP clientUDP = null;
        private ServerTCP serverTCP = null;
        private ServerUDP serverUDP = null;
        private SerialPort serialPort = new SerialPort();
        private uint m_RxTimeout = 0;
        private bool doCalibartion = true;

        public const int JoyMinValue = 0;
        public const int JoyMaxValue = 65535 - 1;
        public const int JoyMidValue = (JoyMinValue + JoyMaxValue)/2;

        public const int PWMminValue = 2000;
        public const int PWMmaxValue = 4000;
        public const int PWMmidValue = (PWMminValue + PWMmaxValue) / 2;
        public const int PWMrange = (PWMmaxValue - PWMminValue);

        public class PlaneAttributes
        {
            public int aileron;
            public int elevator;
            public int rudder;
            public int throttle;
            public int aux;

            public PlaneAttributes()
            {
                aileron = MainForm.PWMmidValue;
                elevator = MainForm.PWMmidValue;
                rudder = MainForm.PWMmidValue;
                throttle = MainForm.PWMmidValue;
                aux = MainForm.PWMmidValue;
            }
            public PlaneAttributes(PlaneAttributes values)
            {
                aileron = values.aileron;
                elevator = values.elevator;
                rudder = values.rudder;
                throttle = values.throttle;
                aux = values.aux;
            }
        }

        public MainForm()
        {
            InitializeComponent();

            //this.listenThread.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            bool joyAttached = false;
            this.Text += "  v" + Application.ProductVersion;
            
            clientTCP = new ClientTCP(this);
            clientUDP = new ClientUDP(this);
            serverTCP = new ServerTCP(this);
            serverUDP = new ServerUDP(this);

            Mode_comboBox.SelectedIndex = 0;

            try
            {
                Set_Joystick_Settings(null, null);          /* Find joystick if there is one and list in Menu -> Settings -> Joystick */
                joyAttached = jyst.Poll(); // check to see if we can get data from joystick
                doCalibartion = true;
                Joystick_timer.Enabled = true;
            }
            catch {
                debug.Append("Joystick failed to init\r\n");
                jystHandler.CenterX = PWMmidValue;
                jystHandler.CenterY = PWMmidValue;
                joyAttached = false;
                MessageBox.Show("Joystick did not Initialize Correctly.\r\nEnsure it is plugged in and restart");
            }

            DiscoverSerialPorts();
            ServiceRegistry(false);

            UpldateRate_numericUpDown_ValueChanged(null, null);
            if (joyAttached && (joystickComboBox.SelectedIndex == -1))
            {
                // if we did not see a joystick in registry but we see one now, use that one
                joystickComboBox.SelectedIndex = 0;
            }

            if ((CommSerialPort_comboBox.Items.Count > 0) && (CommSerialPort_comboBox.SelectedIndex == -1))
            {
                CommSerialPort_comboBox.SelectedIndex = 0;
            }
        }

        private void DiscoverSerialPorts()
        {
            try
            {
                string[] ports = SerialPort.GetPortNames();
                CommSerialPort_comboBox.Items.Clear();
                for (int i = 0; i < ports.Length; i++)
                {
                    CommSerialPort_comboBox.Items.Add(ports[i]);
                    if (ports[i] == serialPort.PortName)
                        CommSerialPort_comboBox.SelectedIndex = i;
                }
            }
            catch (SystemException ex)
            {
                debug.Append("\r\nCrash in DiscoverSerialPorts:\r\n" + ex.ToString());
            }
        }

        private void ServiceRegistry(bool WriteSettings)
        {
            try
            {
                if (WriteSettings)
                {
                    // Joystick
                    Application.UserAppDataRegistry.SetValue("JoySelect", joystickComboBox.SelectedIndex);
                    WriteJoyToRegistry(0);

                    // Connection
                    Application.UserAppDataRegistry.SetValue("CommTypeTCP", CommTypeTCP_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("CommTypeUDP", CommTypeUDP_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("CommTypeSerial", CommTypeSerial_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("IpModeServer", IpModeServer_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("IpModeClient", IpModeClient_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("ClientIP", ClientIP_textBox.Text);
                    Application.UserAppDataRegistry.SetValue("Port", Port_textBox.Text);
                    Application.UserAppDataRegistry.SetValue("UploadInterval", UpldateRate_numericUpDown.Value);
                    Application.UserAppDataRegistry.SetValue("AutoConnect", Connect_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("CommSerialPort", CommSerialPort_comboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("CommSerialBaud", CommSerialBaud_comboBox.SelectedIndex);

                    // User map for joystick
                    Application.UserAppDataRegistry.SetValue("MapAileron", MapAileron_comboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("MapElevator", MapElevator_comboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("MapRudder", MapRudder_comboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("MapThrottle", MapThrottle_comboBox.SelectedIndex);

                    // Misc
                    Application.UserAppDataRegistry.SetValue("DebugIP", ShowRxData_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("CurrentTab", tabControl1.SelectedIndex);
                }
                else
                {
                    // Joystick
                    int joyIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoySelect", -1));
                    if ((joyIndex >= joystickComboBox.Items.Count) && (joyIndex < joystickComboBox.Items.Count))
                        joystickComboBox.SelectedIndex = joyIndex;
                    ReadJoyFromRegistry(0);


                    // Connection
                    CommTypeTCP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeTCP", false));
                    CommTypeUDP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeUDP", false));
                    CommTypeSerial_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeSerial", true));
                    IpModeServer_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeServer", true));
                    IpModeClient_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeClient", false));
                    ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "192.168.1.200").ToString();
                    Port_textBox.Text = Application.UserAppDataRegistry.GetValue("Port", "3104").ToString();
                    UpldateRate_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("UploadInterval", 25));
                    CommSerialBaud_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialBaud", 7));
                    CommSerialPort_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialPort", -1));

                    // Misc
                    ShowRxData_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DebugIP", true));
                    tabControl1.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CurrentTab", 0));

                    // User map for joystick
                    MapAileron_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("MapAileron", 0));
                    MapElevator_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("MapElevator", 1));
                    MapRudder_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("MapRudder", 2));
                    MapThrottle_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("MapThrottle", 3));
                    

                    // always do this one last
                    Connect_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("AutoConnect", false));
                }
            }
            catch (SystemException ex)
            {
                debug.Append("\r\nCrash in ServiceRegistry:\r\n" + ex.ToString()); 
            }
        }


        public void Set_Joystick_Settings(object sender, EventArgs e)
        {
            int i;
            string[] sticks;


            // grab the joystick
            jyst = new JoystickMngr(this.Handle);
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
                debug.Append("Failed assigning Joystick\r\n");
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






        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IsClosing = true;
            ServiceRegistry(true);
            ClientDisconnect_button_Click(null, null);

            try
            {
                jyst.ReleaseJoystick(); // Releases the current Joystick.
            }
            catch { }

        }


        private void Joystick_timer_Tick(object sender, EventArgs e)
        {
            if (joystickComboBox.SelectedIndex < 0)
            {
                // No joystick available, nothing to do.
                return;
            }

            jyst.Poll();
            if (doCalibartion)
            {
                doCalibartion = false;
                jystHandler.CenterX = jyst.state.X;
                jystHandler.CenterY = jyst.state.Y;
            }

            PlaneAttributes planeAttrib = ReassignJystValues(jyst.state);
            JoystickHandler.FbW_Data PwmData = new JoystickHandler.FbW_Data();


            // write joystick to the UI
            if (OverrideAileron_checkBox.Checked)
            {
                PwmData.aileron = Aileron_trackBar.Value;
            }
            else
            {
                PwmData.aileron = jystHandler.ConvertForUI(planeAttrib.aileron, 
                                                        AileronScalar_numericUpDown.Value,
                                                        AileronTrim_numericUpDown.Value,
                                                        InvertAileron_checkBox.Checked);
                Aileron_trackBar.Value = MainForm.Clip(PwmData.aileron, PWMminValue, PWMmaxValue);
            }
            Aileron_label.Text = "Aileron\r" + PwmData.aileron.ToString();


            if (OverrideElevator_checkBox.Checked)
            {
                PwmData.elevator = Elevator_trackBar.Value;
            }
            else
            {
                PwmData.elevator = jystHandler.ConvertForUI(planeAttrib.elevator,
                                                        ElevatorScalar_numericUpDown.Value,
                                                        ElevatorTrim_numericUpDown.Value,
                                                        InvertElevator_checkBox.Checked);
                Elevator_trackBar.Value = MainForm.Clip(PwmData.elevator, PWMminValue, PWMmaxValue);
            }
            Elevator_label.Text = "Elevator\r" + PwmData.elevator.ToString();


            if (OverrideRudder_checkBox.Checked)
            {
                PwmData.rudder = Rudder_trackBar.Value;
            }
            else
            {
                PwmData.rudder = jystHandler.ConvertForUI(planeAttrib.rudder,
                                                        RudderScalar_numericUpDown.Value,
                                                        RudderTrim_numericUpDown.Value,
                                                        InvertRudder_checkBox.Checked);
                Rudder_trackBar.Value = MainForm.Clip(PwmData.rudder, PWMminValue, PWMmaxValue);
            }
            Rudder_label.Text = "Rudder\r" + PwmData.rudder.ToString();


            if (OverrideThrottle_checkBox.Checked)
            {
                PwmData.throttle = Throttle_trackBar.Value;
            }
            else
            {
                PwmData.throttle = jystHandler.ConvertForUI(planeAttrib.throttle,
                                                        ThrottleScalar_numericUpDown.Value,
                                                        ThrottleTrim_numericUpDown.Value,
                                                        InvertThrottle_checkBox.Checked);
                Throttle_trackBar.Value = MainForm.Clip(PwmData.throttle, PWMminValue, PWMmaxValue);
            }
            Throttle_label.Text = "Throttle\r" + PwmData.throttle.ToString();


            switch (Mode_comboBox.SelectedIndex)
            {
                default:
                case 0: PwmData.mode = PWMminValue; break; // Manual
                case 1: PwmData.mode = PWMmidValue; break; // Stabilized
                case 2: PwmData.mode = PWMmaxValue; break; // WayPoint
            }

            byte[] packet = JoystickHandler.CreateTxPacket(PwmData);
            Send(packet);
        }

  

        public string getPublicIP()
        {
            
            string direction;

            System.Net.WebRequest request = System.Net.WebRequest.Create("http://checkip.dyndns.org/");
            System.Net.WebResponse response = request.GetResponse();
            StreamReader stream = new StreamReader(response.GetResponseStream());
            direction = stream.ReadToEnd();
            stream.Close();
            response.Close();

            //Search for the ip in the html
            int first = direction.IndexOf("Address: ") + 9;
            int last = direction.LastIndexOf("</body>");
            direction = direction.Substring(first, last - first);

            return direction;
            
           // return "";
        }

        private void JoyReset_button_Click(object sender, EventArgs e)
        {
            Aileron_trackBar.Value = (Aileron_trackBar.Minimum + Aileron_trackBar.Maximum) / 2;
            Elevator_trackBar.Value = (Elevator_trackBar.Minimum + Elevator_trackBar.Maximum) / 2;
            Throttle_trackBar.Value = (Throttle_trackBar.Minimum + Throttle_trackBar.Maximum) / 2;
            Rudder_trackBar.Value = (Rudder_trackBar.Minimum + Rudder_trackBar.Maximum) / 2;
        }

        private void TextBoxClear_button_Click(object sender, EventArgs e)
        {
            debug_textBox.Text = "";
        }

        private void HouseKeeping_1sec_timer_Tick(object sender, EventArgs e)
        {
            if (m_RxTimeout > 0)
            {
                m_RxTimeout--;
                if (m_RxTimeout == 0)
                    debug.Append("UDP link lost\r\n");
            }
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

        private void Housekeeping_timer_Tick(object sender, EventArgs e)
        {
            if (CommTypeSerial_radioButton.Checked &&
                ShowRxData_checkBox.Checked &&
                serialPort.IsOpen &&
                (serialPort.BytesToRead > 0))
            {
                debug.Append(serialPort.ReadExisting());
            }

            if (debug.Length > 0)
            {
                debug_textBox.AppendText(debug.ToString());
                debug.Length = 0; // google says ".Length=0" is 25% faster than "new StringBuilder()"
            }

            UpdateIsConnected();
        }

        private void ClientDisconnect_button_Click(object sender, EventArgs e)
        {
            Connect_checkBox.Checked = false;
            
            serverTCP.StopListening();
            clientTCP.Disconnect();
            serverUDP.StopListening();
            clientUDP.Disconnect();
            SerialDisconnect();
        }

        public void Send(byte[] data)
        {
            if (CommTypeSerial_radioButton.Checked)
            {
                if (serialPort.IsOpen)
                {
                    try
                    {
                        serialPort.Write(data, 0, data.Length);

                        byte[] checksum = new byte[1];
                        checksum[0] = JoystickHandler.CreateChecksum(data);
                        serialPort.Write(checksum, 0, 1);
                    }
                    catch 
                    {
                        //ClientDisconnect_button_Click(null, null);
                    }
                }
            }
            else if (IpModeServer_radioButton.Checked)
            {
                if (CommTypeTCP_radioButton.Checked)
                    serverTCP.Send(data);
                else if (CommTypeUDP_radioButton.Checked)
                    serverUDP.Send(data);
            }
            else if (IpModeClient_radioButton.Checked)
            {
                if (CommTypeTCP_radioButton.Checked)
                    clientTCP.Send(data);
                else if (CommTypeUDP_radioButton.Checked)
                    clientUDP.Send(data);
            }
        }

        public void ParseRxPacket(byte[] packet, int len)
        {
            m_RxTimeout = RX_TIMEOUT_RESET;
            if (ShowRxData_checkBox.Checked)
            {
                ASCIIEncoding encoder = new ASCIIEncoding();
                debug.Append(encoder.GetString(packet, 0, len));
            }
        }

        private void JoystickSetCenter_button_Click(object sender, EventArgs e)
        {
            doCalibartion = true;
        }

        private void Aileron_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Aileron_trackBar.Value = MainForm.PWMmidValue;
        }
        private void Elevator_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Elevator_trackBar.Value = MainForm.PWMmidValue;
        }
        private void Rudder_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Rudder_trackBar.Value = MainForm.PWMmidValue;
        }
        private void Throttle_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Throttle_trackBar.Value = MainForm.PWMmidValue;
        }

        private void Connect_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            if (Connect_checkBox.Checked == false)
            {
                ClientDisconnect_button_Click(null, null);
                return;
            }

            ushort port = 0;
            try
            {
                port = Convert.ToUInt16(Port_textBox.Text);
            }
            catch
            {
                Port_textBox.Text = "0";
                return;
            }

            if (CommTypeSerial_radioButton.Checked)
            {
                SerialConnect();
            }
            else if (IpModeServer_radioButton.Checked)
            {
                if (CommTypeTCP_radioButton.Checked)
                    serverTCP.StartListening(port);
                else if (CommTypeUDP_radioButton.Checked)
                    serverUDP.StartListening(port);
            }
            else if (IpModeClient_radioButton.Checked)
            {
                if (CommTypeTCP_radioButton.Checked)
                    clientTCP.Connect(ClientIP_textBox.Text, port);
                else if (CommTypeUDP_radioButton.Checked)
                    clientUDP.Connect(ClientIP_textBox.Text, port);
            }
        }


         private void UpdateIsConnected()
        {
            if (CommTypeSerial_radioButton.Checked)
                IsConnected_radioButton.Checked = serialPort.IsOpen;
            else if (CommTypeTCP_radioButton.Checked)
            {
                // TCP
                if (IpModeServer_radioButton.Checked)
                    IsConnected_radioButton.Checked = serverTCP.isConnected();
                else //if (!IpModeServer_radioButton.Checked)
                    IsConnected_radioButton.Checked = clientTCP.isConnected();
            }
            else //if (!CommTypeTCP_radioButton.Checked && IpModeServer_radioButton.Checked)
            {
                // UDP
                IsConnected_radioButton.Checked = (m_RxTimeout != 0);
            }
        }

        private void UpldateRate_numericUpDown_ValueChanged(object sender, EventArgs e)
        {
            Joystick_timer.Interval = Convert.ToInt32(UpldateRate_numericUpDown.Value);

            string interval = (1000 / UpldateRate_numericUpDown.Value).ToString("0.0");
            UploadRate_label.Text = "ms (" + interval + " Hz)";
        }
        private void IpModeServer_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            SetListenButtonText();
            ClientDisconnect_button_Click(null, null);
        }
        private void IpModeClient_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            SetListenButtonText();
            ClientDisconnect_button_Click(null, null);
        }
        private void SetListenButtonText()
        {
            if (CommTypeSerial_radioButton.Checked)
            {
                Connect_checkBox.Text = "Connect";
                IP_groupBox.Enabled = IPmode_groupBox.Enabled = false;
            }
            else
            {
                IP_groupBox.Enabled = IPmode_groupBox.Enabled = true;
                if (IpModeClient_radioButton.Checked)
                    Connect_checkBox.Text = "Connect";
                else if (IpModeServer_radioButton.Checked)
                    Connect_checkBox.Text = "Listen";
            }
            Serial_groupBox.Enabled = !IP_groupBox.Enabled;
        }
        private void IpTypeTCP_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }
        private void IpTypeUDP_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

        private void Port_textBox_TextChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }
        private void ClientIP_textBox_TextChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

        private void SerialDisconnect()
        {
            try
            {
                if (serialPort.IsOpen)
                    serialPort.Close();
            }
            catch (SystemException ex)
            {
                debug.Append("\r\nCrash in SerialDisconnect()\r\n" + ex.ToString());
            }

        }

        private void CommTypeSerial_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            SetListenButtonText();
            ClientDisconnect_button_Click(null, null);
        }

      
        private void SerialConnect()
        {
            SerialDisconnect();

            if (CommSerialPort_comboBox.SelectedIndex >= 0)
            {
                serialPort.PortName = CommSerialPort_comboBox.Items[CommSerialPort_comboBox.SelectedIndex].ToString();
                serialPort.BaudRate = Convert.ToInt32(CommSerialBaud_comboBox.Items[CommSerialBaud_comboBox.SelectedIndex].ToString());

                try
                {
                    SerialDisconnect();
                    serialPort.Open();
                }
                catch (SystemException ex)
                {
                    debug.Append("\r\nCrash in SerialConnect, probably could not open the port.\r\n" + 
                    "Try Disconnecting and re-Connecting:\r\n\r\n" + ex.ToString());
                }
                UpdateIsConnected();
            }
        }

        private void InvertAileron_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            jystHandler.InvertX = InvertAileron_checkBox.Checked;
        }
        private void InvertElevator_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            jystHandler.InvertY = InvertElevator_checkBox.Checked;
        }
        private void InvertRudder_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            jystHandler.InvertRudder = InvertRudder_checkBox.Checked;
        }
        private void InvertThrottle_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            jystHandler.InvertThrottle = InvertThrottle_checkBox.Checked;
        }

        public static int Clip(int value, int min, int max)
        {
            if (value >= max)
                return max;
            else if (value <= min)
                return min;
            else
                return value;
        }

        public PlaneAttributes ReassignJystValues(JoystickState joyState)
        {
            PlaneAttributes planeAttrib = new PlaneAttributes();

            switch (MapAileron_comboBox.SelectedIndex)
            {
                case 0: planeAttrib.aileron = joyState.X; break; // X
                case 1: planeAttrib.aileron = joyState.Y; break;// Y
                case 2: planeAttrib.aileron = joyState.Z; break;// Z
                case 3: planeAttrib.aileron = joyState.GetSlider()[0]; break;// Slider1
                case 4: planeAttrib.aileron = joyState.GetSlider()[1]; break;// Slider2
                default:
                    break;
            }

            switch (MapElevator_comboBox.SelectedIndex)
            {
                case 0: planeAttrib.elevator = joyState.X; break; // X
                case 1: planeAttrib.elevator = joyState.Y; break;// Y
                case 2: planeAttrib.elevator = joyState.Z; break;// Z
                case 3: planeAttrib.elevator = joyState.GetSlider()[0]; break;// Slider1
                case 4: planeAttrib.elevator = joyState.GetSlider()[1]; break;// Slider2
                default:
                    break;
            }

            switch (MapRudder_comboBox.SelectedIndex)
            {
                case 0: planeAttrib.rudder = joyState.X; break; // X
                case 1: planeAttrib.rudder = joyState.Y; break;// Y
                case 2: planeAttrib.rudder = joyState.Z; break;// Z
                case 3: planeAttrib.rudder = joyState.GetSlider()[0]; break;// Slider1
                case 4: planeAttrib.rudder = joyState.GetSlider()[1]; break;// Slider2
                default:
                    break;
            }

            switch (MapThrottle_comboBox.SelectedIndex)
            {
                case 0: planeAttrib.throttle = joyState.X; break; // X
                case 1: planeAttrib.throttle = joyState.Y; break;// Y
                case 2: planeAttrib.throttle = joyState.Z; break;// Z
                case 3: planeAttrib.throttle = joyState.GetSlider()[0]; break;// Slider1
                case 4: planeAttrib.throttle = joyState.GetSlider()[1]; break;// Slider2
                default:
                    break;
            }

            return planeAttrib;
        }

        private void PublicIP_button_Click(object sender, EventArgs e)
        {
            string publicIP = getPublicIP();
            debug.Append("Public IP: " + publicIP + "\r\n");
            MessageBox.Show("Your Public IP is: " + publicIP);
        }

        private void Save1_button_Click(object sender, EventArgs e)
        {
            WriteJoyToRegistry(1);
        }

        private void Load1_button_Click(object sender, EventArgs e)
        {
            ReadJoyFromRegistry(1);
        }

        private void Save2_button_Click(object sender, EventArgs e)
        {
            WriteJoyToRegistry(2);
        }

        private void Load2_button_Click(object sender, EventArgs e)
        {
            ReadJoyFromRegistry(2);
        }

        private void WriteJoyToRegistry(int index)
        {
            string i = "_" + index.ToString();
            Application.UserAppDataRegistry.SetValue("JoyInvertX" + i, InvertAileron_checkBox.Checked);
            Application.UserAppDataRegistry.SetValue("JoyInvertY" + i, InvertElevator_checkBox.Checked);
            Application.UserAppDataRegistry.SetValue("JoyInvertR" + i, InvertRudder_checkBox.Checked);
            Application.UserAppDataRegistry.SetValue("JoyInvertT" + i, InvertThrottle_checkBox.Checked);
            Application.UserAppDataRegistry.SetValue("JoyTrimX" + i, AileronTrim_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyTrimY" + i, ElevatorTrim_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyTrimR" + i, RudderTrim_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyTrimT" + i, ThrottleTrim_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyScalarX" + i, AileronScalar_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyScalarY" + i, ElevatorScalar_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyScalarR" + i, RudderScalar_numericUpDown.Value);
            Application.UserAppDataRegistry.SetValue("JoyScalarT" + i, ThrottleScalar_numericUpDown.Value);
        }
        private void ReadJoyFromRegistry(int index)
        {
            string i = "_" + index.ToString();
            InvertAileron_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertX" + i, false));
            InvertElevator_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertY" + i, false));
            InvertRudder_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertR" + i, false));
            InvertThrottle_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertT" + i, false));
            AileronTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimX" + i, 0));
            ElevatorTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimY" + i, 0));
            RudderTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimR" + i, 0));
            ThrottleTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimT" + i, 0));
            AileronScalar_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyScalarX" + i, 1));
            ElevatorScalar_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyScalarY" + i, 1));
            RudderScalar_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyScalarR" + i, 1));
            ThrottleScalar_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyScalarT" + i, 1));
        }

        private void AileronTrim_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                AileronTrim_numericUpDown.Value = 0;
        }
        private void ElevatorTrim_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                ElevatorTrim_numericUpDown.Value = 0;
        }
        private void RudderTrim_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                RudderTrim_numericUpDown.Value = 0;
        }
        private void ThrottleTrim_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                ThrottleTrim_numericUpDown.Value = 0;
        }

        private void AileronScalar_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                AileronScalar_numericUpDown.Value = 1;
        }
        private void ElevatorScalar_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                ElevatorScalar_numericUpDown.Value = 1;
        }
        private void RudderScalar_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                RudderScalar_numericUpDown.Value = 1;
        }
        private void ThrottleScalar_numericUpDown_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                ThrottleScalar_numericUpDown.Value = 1;
        }

    } // class
} // namespace
