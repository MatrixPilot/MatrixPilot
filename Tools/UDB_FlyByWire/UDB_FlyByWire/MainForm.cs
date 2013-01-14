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
        //public string data = "";
        public bool IsClosing = false;

        private UDB_FlyByWire.JoystickMngr jyst;
        private DeviceList gameControllerList;
        private JoystickHandler jystHandler = new JoystickHandler();

        public StringBuilder debug = new StringBuilder("");
        private ClientTCP clientTCP = null;
        private ClientUDP clientUDP = null;
        private ServerTCP serverTCP = null;
        private ServerUDP serverUDP = null;
        private SerialPort serialPort = new SerialPort();

        public MainForm()
        {
            InitializeComponent();

            //this.listenThread.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Text += "  v" + Application.ProductVersion;
            
            clientTCP = new ClientTCP(this);
            clientUDP = new ClientUDP(this);
            serverTCP = new ServerTCP(this);
            serverUDP = new ServerUDP(this);


            Set_Joystick_Settings(null, null);          /* Find joystick if there is one and list in Menu -> Settings -> Joystick */
            Mode_comboBox.SelectedIndex = 0;

            try
            {
                gameControllerList = Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.AttachedOnly);
                if (gameControllerList.Count > 0)
                {
                    jyst.Poll();
                    jystHandler.CenterX = jyst.state.X;
                    jystHandler.CenterY = jyst.state.Y;
                }
                else
                {
                    debug.Append("No Joystick found\r\n");
                    joystickComboBox.Items.Clear();
                }
            }
            catch { }

            DiscoverSerialPorts();

            ServiceRegistry(false);

            UpldateRate_numericUpDown_ValueChanged(null, null);
            if ((joystickComboBox.SelectedIndex == -1) && (gameControllerList.Count > 0))
                joystickComboBox.SelectedIndex = 0;
            if ((CommSerialPort_comboBox.Items.Count > 0) && (CommSerialPort_comboBox.SelectedIndex == -1))
                CommSerialPort_comboBox.SelectedIndex = 0;
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
                    Application.UserAppDataRegistry.SetValue("JoyCalX", jystHandler.CenterX);
                    Application.UserAppDataRegistry.SetValue("JoyCalY", jystHandler.CenterY);
                    Application.UserAppDataRegistry.SetValue("JoySelect", joystickComboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("JoyOverride", OverrideJoy_checkBox.Checked);

                    Application.UserAppDataRegistry.SetValue("JoyInvertX", InvertAileron_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("JoyInvertY", InvertElevator_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("JoyInvertR", InvertRudder_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("JoyInvertT", InvertThrottle_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("JoyTrimX", AileronTrim_numericUpDown.Value);
                    Application.UserAppDataRegistry.SetValue("JoyTrimY", ElevatorTrim_numericUpDown.Value);
                    Application.UserAppDataRegistry.SetValue("JoyTrimR", RudderTrim_numericUpDown.Value);
                    Application.UserAppDataRegistry.SetValue("JoyTrimT", ThrottleTrim_numericUpDown.Value);


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
                    
                    
                    // Misc
                    Application.UserAppDataRegistry.SetValue("DebugIP", IpDebug_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("DebugJoy", JoyStickDebug_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("CurrentTab", tabControl1.SelectedIndex);
                }
                else
                {
                    // Joystick
                    jystHandler.CenterX = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoyCalX", 0xFFFF / 2));
                    jystHandler.CenterY = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoyCalY", 0xFFFF / 2));
                    joystickComboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoySelect", -1));
                    OverrideJoy_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyOverride", false));
                    InvertAileron_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertX", false));
                    InvertElevator_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertY", false));
                    InvertRudder_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertR", false));
                    InvertThrottle_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertT", false));
                    AileronTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimX", 0));
                    ElevatorTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimY", 0));
                    RudderTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimR", 0));
                    ThrottleTrim_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("JoyTrimT", 0));


                    // Connection
                    CommTypeTCP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeTCP", false));
                    CommTypeUDP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeUDP", false));
                    CommTypeSerial_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("CommTypeSerial", true));
                    IpModeServer_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeServer", true));
                    IpModeClient_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeClient", false));
                    ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "192.168.11.200").ToString();
                    Port_textBox.Text = Application.UserAppDataRegistry.GetValue("Port", "3003").ToString();
                    UpldateRate_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("UploadInterval", 25));
                    CommSerialBaud_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialBaud", 7));
                    CommSerialPort_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialPort", -1));

                    // Misc
                    IpDebug_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DebugIP", true));
                    JoyStickDebug_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DebugJoy", true));
                    tabControl1.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CurrentTab", 0));

                    // always do this oen last
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
            JoystickHandler.FbW_Data PwmData = new JoystickHandler.FbW_Data();
            JoystickHandler.FbW_Data PercentData = new JoystickHandler.FbW_Data();

            if (OverrideJoy_checkBox.Checked)
            {
                PercentData.m_aileron = Aileron_trackBar.Value;
                PercentData.m_elevator = Elevator_trackBar.Value;
                PercentData.m_throttle = Throttle_trackBar.Value;
            }
            else if (joystickComboBox.SelectedIndex < 0)
            {
                // No joystick available
                return;
            }
            else
            {
                jyst.Poll();
                PercentData = jystHandler.ConvertToPercent(jyst.state);

                // write joystick to the UI
                Aileron_trackBar.Value = PercentData.m_aileron;
                Elevator_trackBar.Value = PercentData.m_elevator;
                Throttle_trackBar.Value = PercentData.m_throttle;
            }

            // don't have an input for this on the joystick so always do manual
            PercentData.m_rudder = Rudder_trackBar.Value + Convert.ToInt32(RudderTrim_numericUpDown.Value * RudderScalar_numericUpDown.Value);

            PercentData.m_aileron += Convert.ToInt32(AileronTrim_numericUpDown.Value * AileronScalar_numericUpDown.Value);
            PercentData.m_elevator += Convert.ToInt32(ElevatorTrim_numericUpDown.Value * ElevatorScalar_numericUpDown.Value);
            PercentData.m_throttle += Convert.ToInt32(ThrottleTrim_numericUpDown.Value * ThrottleScalar_numericUpDown.Value);

            PwmData = JoystickHandler.ConvertToPWM(PercentData, Mode_comboBox.SelectedIndex);
            byte[] packet = JoystickHandler.CreateTxPacket(PwmData);
            Send(packet);
        }        

  

        public string getPublicIP()
        {
            /*
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
             * */
            return "";
        }

        private void JoyReset_button_Click(object sender, EventArgs e)
        {
            Aileron_trackBar.Value = 0;
            Elevator_trackBar.Value = 0;
            Throttle_trackBar.Value = 0;
            Rudder_trackBar.Value = 0;
        }

        private void TextBoxClear_button_Click(object sender, EventArgs e)
        {
            debug_textBox.Text = "";
        }

        private void HouseKeeping_1sec_timer_Tick(object sender, EventArgs e)
        {
            UpdateIsConnected();
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
            if (IpDebug_checkBox.Checked)
            {
                ASCIIEncoding encoder = new ASCIIEncoding();
                debug.Append(encoder.GetString(packet, 0, len));
            }
        }

        private void JoystickSetCenter_button_Click(object sender, EventArgs e)
        {
            if (gameControllerList.Count > 0)
            {
                jyst.Poll();
                jystHandler.CenterX = jyst.state.X;
                jystHandler.CenterY = jyst.state.Y;
            }
        }

        private void IpDebug_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            JoyStickDebug_checkBox.Checked = !IpDebug_checkBox.Checked;
        }
        private void JoyStickDebug_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            IpDebug_checkBox.Checked = !JoyStickDebug_checkBox.Checked;
        }

        private void Aileron_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Aileron_trackBar.Value = 0;
        }
        private void Elevator_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Elevator_trackBar.Value = 0;
        }
        private void Rudder_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Rudder_trackBar.Value = 0;
        }
        private void Throttle_trackBar_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                Throttle_trackBar.Value = 0;
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


        private void IsConnected_radioButton_MouseClick(object sender, MouseEventArgs e)
        {
            UpdateIsConnected();
        }

        private void UpdateIsConnected()
        {
            if (CommTypeSerial_radioButton.Checked)
                IsConnected_radioButton.Checked = serialPort.IsOpen;
            else if (CommTypeTCP_radioButton.Checked && IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = serverTCP.isConnected();
            else if (CommTypeTCP_radioButton.Checked && !IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = clientTCP.isConnected();
            else if (!CommTypeTCP_radioButton.Checked && IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = serverUDP.isConnected();
            else if (!CommTypeTCP_radioButton.Checked && !IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = clientUDP.isConnected();
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
                debug.Append("\r\nCrash in CommTypeSerial_radioButton_CheckedChanged:\r\n" + ex.ToString());
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
                    serialPort.Open();
                }
                catch (SystemException ex)
                {
                    debug.Append("\r\nCrash in SerialConnect, probably could not open the port:\r\n" + ex.ToString());
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

    } // class
} // namespace
