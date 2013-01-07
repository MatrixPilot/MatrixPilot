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
                    joystickComboBox.SelectedIndex = -1;
                }
            }
            catch { }

            ServiceRegistry(false);
            UpldateRate_numericUpDown_ValueChanged(null, null);
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
                    Application.UserAppDataRegistry.SetValue("JoyInvertY", InvertY_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("JoySelect", joystickComboBox.SelectedIndex);
                    Application.UserAppDataRegistry.SetValue("JoyOverride", OverrideJoy_checkBox.Checked);

                    // Connection
                    Application.UserAppDataRegistry.SetValue("IpTypeTCP", IpTypeTCP_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("IpTypeUDP", IpTypeUDP_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("IpModeServer", IpModeServer_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("IpModeClient", IpModeClient_radioButton.Checked);
                    Application.UserAppDataRegistry.SetValue("ClientIP", ClientIP_textBox.Text);
                    Application.UserAppDataRegistry.SetValue("Port", Port_textBox.Text);
                    Application.UserAppDataRegistry.SetValue("UploadInterval", UpldateRate_numericUpDown.Value);
                    
                    // Misc
                    Application.UserAppDataRegistry.SetValue("DebugIP", IpDebug_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("DebugJoy", JoyStickDebug_checkBox.Checked);
                    Application.UserAppDataRegistry.SetValue("CurrentTab", tabControl1.SelectedIndex);
                }
                else
                {

                    tabControl1.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CurrentTab", 2));

                    // Joystick
                    jystHandler.CenterX = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoyCalX", 0xFFFF / 2));
                    jystHandler.CenterY = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoyCalY", 0xFFFF / 2));
                    InvertY_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyInvertY", true));
                    joystickComboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("JoySelect", -1));
                    OverrideJoy_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JoyOverride", false));


                    // Connection
                    IpTypeTCP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpTypeTCP", true));
                    IpTypeUDP_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpTypeUDP", false));
                    IpModeServer_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeServer", true));
                    IpModeClient_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("IpModeClient", false));
                    ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "192.168.11.200").ToString();
                    Port_textBox.Text = Application.UserAppDataRegistry.GetValue("Port", "3003").ToString();
                    UpldateRate_numericUpDown.Value = Convert.ToDecimal(Application.UserAppDataRegistry.GetValue("UploadInterval", 25));

                    // Misc
                    IpDebug_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DebugIP", true));
                    JoyStickDebug_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DebugJoy", true));
                    tabControl1.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CurrentTab", 0));
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
                PercentData.m_mode = 0;
                PercentData.m_rudder = Rudder_trackBar.Value;
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

                // don't have an input for this on the joystick so allow manual
                PercentData.m_rudder = Rudder_trackBar.Value;

                // write joystick to the UI
                Aileron_trackBar.Value = PercentData.m_aileron;
                Elevator_trackBar.Value = PercentData.m_elevator;
                Throttle_trackBar.Value = PercentData.m_throttle;
            }

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
        }

        public void Send(byte[] data)
        {
            if (IpModeServer_radioButton.Checked)
            {
                if (IpTypeTCP_radioButton.Checked)
                    serverTCP.Send(data);
                else if (IpTypeUDP_radioButton.Checked)
                    serverUDP.Send(data);
            }
            else if (IpModeClient_radioButton.Checked)
            {
                if (IpTypeTCP_radioButton.Checked)
                    clientTCP.Send(data);
                else if (IpTypeUDP_radioButton.Checked)
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

        private void InvertY_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            jystHandler.InvertY = InvertY_checkBox.Checked;
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

            ushort port = Convert.ToUInt16(Port_textBox.Text);

            if (IpModeServer_radioButton.Checked)
            {
                if (IpTypeTCP_radioButton.Checked)
                    serverTCP.StartListening(port);
                else if (IpTypeUDP_radioButton.Checked)
                    serverUDP.StartListening(port);
            }
            else if (IpModeClient_radioButton.Checked)
            {
                if (IpTypeTCP_radioButton.Checked)
                    clientTCP.Connect(ClientIP_textBox.Text, port);
                else if (IpTypeUDP_radioButton.Checked)
                    clientUDP.Connect(ClientIP_textBox.Text, port);
            }
        }


        private void IsConnected_radioButton_MouseClick(object sender, MouseEventArgs e)
        {
            UpdateIsConnected();
        }

        private void UpdateIsConnected()
        {
            if (IpTypeTCP_radioButton.Checked && IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = serverTCP.isConnected();
            else if (IpTypeTCP_radioButton.Checked && !IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = clientTCP.isConnected();
            else if (!IpTypeTCP_radioButton.Checked && IpModeServer_radioButton.Checked)
                IsConnected_radioButton.Checked = serverUDP.isConnected();
            else if (!IpTypeTCP_radioButton.Checked && !IpModeServer_radioButton.Checked)
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
            if (IpModeServer_radioButton.Checked)
                Connect_checkBox.Text = "Listen";
            ClientDisconnect_button_Click(null, null);
        }
        private void IpModeClient_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            if (IpModeClient_radioButton.Checked)
                Connect_checkBox.Text = "Connect";
            ClientDisconnect_button_Click(null, null);
        }
        private void IpTypeTCP_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }
        private void IpTypeUDP_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

      

    }
}
