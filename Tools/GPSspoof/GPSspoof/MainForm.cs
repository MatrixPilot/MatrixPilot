using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace GPSspoof
{
    public partial class MainForm : Form
    {
        private ClientTCP clientTCP = null;
        public bool IsClosing = false;
        public StringBuilder debug = new StringBuilder("");

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.Text += "  v" + Application.ProductVersion;

            clientTCP = new ClientTCP(this);
            ServiceRegistry(false);

        }

        private void ServiceRegistry(bool WriteSettings)
        {
            if (WriteSettings)
            {



                Application.UserAppDataRegistry.SetValue("Lat", Lat_textBox.Text);
                Application.UserAppDataRegistry.SetValue("Long", Long_textBox.Text);
                Application.UserAppDataRegistry.SetValue("Alt", Alt_textBox.Text);
                Application.UserAppDataRegistry.SetValue("LatInc", LatInc_textBox.Text);
                Application.UserAppDataRegistry.SetValue("LongInc", LongInc_textBox.Text);
                Application.UserAppDataRegistry.SetValue("AltInc", AltInc_textBox.Text);
                Application.UserAppDataRegistry.SetValue("SendDataRepeat", SendDataRepeat_checkBox.Checked);
                Application.UserAppDataRegistry.SetValue("SendDataInterval", SendDataInterval_textBox.Text);

                Application.UserAppDataRegistry.SetValue("LatReset", LatReset_textBox.Text);
                Application.UserAppDataRegistry.SetValue("LongReset", LongReset_textBox.Text);
                Application.UserAppDataRegistry.SetValue("AltReset", AltReset_textBox.Text);

                Application.UserAppDataRegistry.SetValue("SpoofMode", SpoofMode_comboBox.SelectedIndex);
                Application.UserAppDataRegistry.SetValue("ClientIP", ClientIP_textBox.Text);
                Application.UserAppDataRegistry.SetValue("Port", Port_textBox.Text);
                Application.UserAppDataRegistry.SetValue("AutoConnect", Connect_checkBox.Checked);
            }
            else
            {
                Lat_textBox.Text = Application.UserAppDataRegistry.GetValue("Lat", "0").ToString();
                Long_textBox.Text = Application.UserAppDataRegistry.GetValue("Long", "0").ToString();
                Alt_textBox.Text = Application.UserAppDataRegistry.GetValue("Alt", "0").ToString();
                LatInc_textBox.Text = Application.UserAppDataRegistry.GetValue("LatInc", "0").ToString();
                LongInc_textBox.Text = Application.UserAppDataRegistry.GetValue("LongInc", "0").ToString();
                AltInc_textBox.Text = Application.UserAppDataRegistry.GetValue("AltInc", "0").ToString();
                SendDataRepeat_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("SendDataRepeat", false));
                SendDataInterval_textBox.Text = Application.UserAppDataRegistry.GetValue("SendDataInterval", 5000).ToString();

                LatReset_textBox.Text = Application.UserAppDataRegistry.GetValue("LatReset", "0").ToString();
                LongReset_textBox.Text = Application.UserAppDataRegistry.GetValue("LongReset", "0").ToString();
                AltReset_textBox.Text = Application.UserAppDataRegistry.GetValue("AltReset", "0").ToString();
                
                SpoofMode_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("SpoofMode", 0));
                ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "192.168.1.200").ToString();
                Port_textBox.Text = Application.UserAppDataRegistry.GetValue("Port", "3007").ToString();

                // always do this one last
                Connect_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("AutoConnect", false));
            }

            ConfigureButtonsAndLabels();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            IsClosing = true;
            ServiceRegistry(true);
            ClientDisconnect_button_Click(null, null);

        }

        private void HouseKeeping_1sec_timer_Tick(object sender, EventArgs e)
        {
            UpdateIsConnected();
        }

        public void Send(byte[] data)
        {
            clientTCP.Send(data);
        }

        private void IsConnected_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateIsConnected();
        }

        private void ClientDisconnect_button_Click(object sender, EventArgs e)
        {
            Connect_checkBox.Checked = false;
            clientTCP.Disconnect();
        }

        private void ClientIP_textBox_TextChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

        private void Port_textBox_TextChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

        private void UpdateIsConnected()
        {
            IsConnected_radioButton.Checked = clientTCP.isConnected();
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

            clientTCP.Connect(ClientIP_textBox.Text, port);
        }

        public void HouseKeeping_100ms_timer_Tick(object sender, EventArgs e)
        {
            if (debug.Length > 0)
            {
                debug_textBox.AppendText(debug.ToString());
                debug.Length = 0; // google says ".Length=0" is 25% faster than "new StringBuilder()"
            }
        }

        public void ParseRxPacket(byte[] packet, int len)
        {
            ASCIIEncoding encoder = new ASCIIEncoding();
            debug.Append(encoder.GetString(packet, 0, len));
        }

        private void TextBoxClear_button_Click(object sender, EventArgs e)
        {
            debug_textBox.Text = "";
        }

        private void SpoofMode_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ConfigureButtonsAndLabels();
        }

        private void SendDataInterval_textBox_TextChanged(object sender, EventArgs e)
        {
            try
            {
                int interval;
                interval = Convert.ToInt32(SendDataInterval_textBox.Text);
                if (interval < 100)
                {
                    SendDataInterval_textBox.Text = "100";
                    interval = 100;
                }

                SendData_timer.Interval = interval;
            }
            catch
            {
                SendDataInterval_textBox.Text = "5000";
            }
        }

        private void SendData_timer_Tick(object sender, EventArgs e)
        {
            if (clientTCP.isConnected() == false)
                return;

            int latOffset = Convert.ToInt32(LatOffset_textBox.Text);
            latOffset += Convert.ToInt32(LatInc_textBox.Text);
            LatOffset_textBox.Text = latOffset.ToString();

            int longOffset = Convert.ToInt32(LongOffset_textBox.Text);
            longOffset += Convert.ToInt32(LongInc_textBox.Text);
            LongOffset_textBox.Text = longOffset.ToString();

            int altOffset = Convert.ToInt32(AltOffset_textBox.Text);
            altOffset += Convert.ToInt32(AltInc_textBox.Text);
            AltOffset_textBox.Text = altOffset.ToString();

            SendSpoofPacket();
        }

        private void SendDataRepeat_checkBox_CheckedChanged(object sender, EventArgs e)
        {
             ConfigureButtonsAndLabels();
        }

        private void SendDataOnce_button_Click(object sender, EventArgs e)
        {
            SendSpoofPacket();
        }

        private void SendSpoofPacket()
        {
            string str = "";
            str += SpoofMode_comboBox.SelectedIndex.ToString();

            switch (SpoofMode_comboBox.SelectedIndex)
            {
                case 0:
                default:
                    return;
                case 1: //override
                    str += "," + Lat_textBox.Text;
                    str += "," + Long_textBox.Text;
                    str += "," + Alt_textBox.Text;
                    break;
                case 2: //offset
                    str += "," + LatOffset_textBox.Text;
                    str += "," + LongOffset_textBox.Text;
                    str += "," + AltOffset_textBox.Text;
                    break;
            }

            str += "\r";

            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            byte[] packet = System.Text.Encoding.ASCII.GetBytes(str);
            Send(packet);
        }


        private void ConfigureButtonsAndLabels()
        {
            SendData_timer.Enabled = SendDataRepeat_checkBox.Checked;

            switch (SpoofMode_comboBox.SelectedIndex)
            {
                case 0: // disabled
                    SendDataOnce_button.Enabled = false;
                    SendDataInterval_textBox.Enabled = false;
                    break;

                case 1: // Override
                    SendDataOnce_button.Enabled = !SendDataRepeat_checkBox.Checked;
                    SendDataInterval_textBox.Enabled = SendDataRepeat_checkBox.Checked;
                    break;

                case 2: // Offset
                    SendDataOnce_button.Enabled = !SendDataRepeat_checkBox.Checked;
                    SendDataInterval_textBox.Enabled = SendDataRepeat_checkBox.Checked;
                    break;

                default:
                    SpoofMode_comboBox.SelectedIndex = 0;
                    break;
            }

            Lat_textBox.Enabled = Long_textBox.Enabled = Alt_textBox.Enabled = (SpoofMode_comboBox.SelectedIndex == 1);
            LatInc_textBox.Enabled = LongInc_textBox.Enabled = AltInc_textBox.Enabled = ((SpoofMode_comboBox.SelectedIndex == 2) && SendDataRepeat_checkBox.Checked);
            LatOffset_textBox.Enabled = LongOffset_textBox.Enabled = AltOffset_textBox.Enabled = (SpoofMode_comboBox.SelectedIndex == 2);

        }

        private void CopyLatLong_button_Click(object sender, EventArgs e)
        {
            Lat_textBox.Text = LatReset_textBox.Text;
            Long_textBox.Text = LongReset_textBox.Text;
            Alt_textBox.Text = AltReset_textBox.Text;
        }

        private void ResetLatLong_button_Click(object sender, EventArgs e)
        {
            LatReset_textBox.Text = "374124664";
            LongReset_textBox.Text = "-1219950467";
            AltReset_textBox.Text = "200";
        }


    } // clasee
} // namespace
