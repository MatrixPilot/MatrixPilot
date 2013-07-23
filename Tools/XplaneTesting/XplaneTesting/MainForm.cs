using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace XplaneTesting
{
    public partial class MainForm : Form
    {
        public StringBuilder debug = new StringBuilder("");

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.Text += "  v" + Application.ProductVersion;

        }
        private void ServiceRegistry(bool WriteSettings)
        {
            if (WriteSettings)
            {
                Application.UserAppDataRegistry.SetValue("ClientIP", ClientIP_textBox.Text);
                Application.UserAppDataRegistry.SetValue("Port", Port_textBox.Text);
            }
            else
            {
                ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "192.168.1.200").ToString();
                Port_textBox.Text = Application.UserAppDataRegistry.GetValue("Port", "49000").ToString();
            }
        }

        private void PublicIP_button_Click(object sender, EventArgs e)
        {
            string publicIP = getPublicIP();
            debug.Append("Public IP: " + publicIP + "\r\n");
            MessageBox.Show("Your Public IP is: " + publicIP);
        }

        private void Housekeeping_timer_Tick(object sender, EventArgs e)
        {
            if (debug.Length > 0)
            {
                debug_textBox.AppendText(debug.ToString());
                debug.Length = 0; // google says ".Length=0" is 25% faster than "new StringBuilder()"
            }
        }

        public string getPublicIP()
        {

            string direction;

            System.Net.WebRequest request = System.Net.WebRequest.Create("http://checkip.dyndns.org/");
            System.Net.WebResponse response = request.GetResponse();
            System.IO.StreamReader stream = new System.IO.StreamReader(response.GetResponseStream());
            direction = stream.ReadToEnd();
            stream.Close();
            response.Close();

            //Search for the ip in the html
            int first = direction.IndexOf("Address: ") + 9;
            int last = direction.LastIndexOf("</body>");
            direction = direction.Substring(first, last - first);

            return direction;
        }

        private void Debug1_button_Click(object sender, EventArgs e)
        {
            string ip = ClientIP_textBox.Text;
            UInt16 port = Convert.ToUInt16(Port_textBox.Text);
            sendPacket(ip, port, null);
        }

        private void sendPacket(string ip, UInt16 port, byte[] data)
        {
            if ((data != null) && (data.Length > 0))
            {
                System.Net.Sockets.UdpClient client = new System.Net.Sockets.UdpClient(ip, port);
                client.Send(data, data.Length);
            }
        }
    }
}
