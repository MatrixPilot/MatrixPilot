using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;


namespace LogoCmdSender
{
    public partial class MainForm : Form
    {
/*
// Define the Low-level Commands
//							   cmd,fly,param,sub,x
#define _REPEAT(n, pr)			{1,	0,	pr,	0,	n},
#define _END					{1,	0,	0,	1,	0},
#define _ELSE					{1,	0,	0,	3,	0},
#define _TO(fn)					{1,	0,	0,	2,	fn},

#define _DO(fn, x, pr)			{2,	0,	pr,	fn, x},
#define _EXEC(fn, x, pr)		{10,0,	pr,	fn, x},

#define _FD(x, fl, pr)			{3,	fl,	pr,	0,	x},

#define _RT(x, pr)				{4,	0,	pr, 0,	x},
#define _SET_ANGLE(x, pr)		{4,	0,	pr, 1,	x},
#define _USE_CURRENT_ANGLE		{4,	0,	0,	2,	0},
#define _USE_ANGLE_TO_GOAL		{4,	0,	0,	3,	0},

#define _MV_X(x, fl, pr)		{5,	fl,	pr,	0,	x},
#define _SET_X(x, fl, pr)		{5,	fl,	pr,	1,	x},
#define _MV_Y(y, fl, pr)		{5,	fl,	pr,	2,	y},
#define _SET_Y(y, fl, pr)		{5,	fl,	pr,	3,	y},
#define _MV_Z(z, fl, pr)		{5,	fl,	pr,	4,	z},
#define _SET_Z(z, fl, pr)		{5,	fl,	pr,	5,	z},
#define _USE_CURRENT_POS(fl)	{5, fl,	0,	6,	0},
#define _HOME(fl)				{5,	fl,	0,	7,	0},

#define _SET_ABS_VAL_HIGH(x)	{5,	0,	0,	8,	x}, // Set the high and then low words for X and
#define _SET_ABS_X_LOW(x)		{5,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
#define _SET_ABS_Y_LOW(y, fl)	{5,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)

#define _FLAG_ON(f)				{6,	0,	0,	0,	f},
#define _FLAG_OFF(f)			{6,	0,	0,	1,	f},
#define _FLAG_TOGGLE(f)			{6,	0,	0,	2,	f},

#define _PEN_UP					{7,	0,	0,	0,	0},
#define _PEN_DOWN				{7,	1,	0,	1,	0},
#define _PEN_TOGGLE				{7,	0,	0,	2,	0},

#define _SET_TURTLE(x)			{8,	0,	0,	0,	x},

#define _PARAM_SET(x)			{9,	0,	0,	0,	x},
#define _PARAM_ADD(x)			{9,	0,	0,	1,	x},
#define _PARAM_MUL(x)			{9,	0,	0,	2,	x},
#define _PARAM_DIV(x)			{9,	0,	0,	3,	x},

#define _SPEED_INCREASE(s, pr)	{11,0,	pr,	0,	s},
#define _SET_SPEED(s, pr)		{11,0,	pr,	1,	s},

#define _SET_INTERRUPT(fn)		{12,0,	0,	1,	fn},
#define _CLEAR_INTERRUPT		{12,0,	0,	0,	0},

#define _LOAD_TO_PARAM(val)		{13,0,	0,	val,0},

#define _IF_EQ(val, x, pr)		{14,0,	pr,	val,x},
#define _IF_NE(val, x, pr)		{15,0,	pr,	val,x},
#define _IF_GT(val, x, pr)		{16,0,	pr,	val,x},
#define _IF_LT(val, x, pr)		{17,0,	pr,	val,x},
#define _IF_GE(val, x, pr)		{18,0,	pr,	val,x},
#define _IF_LE(val, x, pr)		{19,0,	pr,	val,x},


// Define the High-level Commands
#define FD(x)				_FD(x, 1, 0)
#define BK(x)				_FD(-x, 1, 0)
#define FD_PARAM			_FD(1, 1, 1)
#define BK_PARAM			_FD(-1, 1, 1)

#define RT(x)				_RT(x, 0)
#define LT(x)				_RT(-x, 0)
#define SET_ANGLE(x)		_SET_ANGLE(x, 0)
#define RT_PARAM			_RT(1, 1)
#define LT_PARAM			_RT(-1, 1)
#define SET_ANGLE_PARAM		_SET_ANGLE(0, 1)
#define USE_CURRENT_ANGLE	_USE_CURRENT_ANGLE
#define USE_ANGLE_TO_GOAL	_USE_ANGLE_TO_GOAL

#define EAST(x)				_MV_X(x, 1, 0)
#define WEST(x)				_MV_X(-x, 1, 0)
#define SET_X_POS(x)		_SET_X(x, 1, 0)
#define EAST_PARAM			_MV_X(1, 1, 1)
#define WEST_PARAM			_MV_X(-1, 1, 1)
#define SET_X_POS_PARAM		_SET_X(1, 1, 1)
#define USE_CURRENT_POS		_USE_CURRENT_POS(1)

#define NORTH(y)			_MV_Y(y, 1, 0)
#define SOUTH(y)			_MV_Y(-y, 1, 0)
#define SET_Y_POS(y)		_SET_Y(y, 1, 0)
#define NORTH_PARAM			_MV_Y(1, 1, 1)
#define SOUTH_PARAM			_MV_Y(-1, 1, 1)
#define SET_Y_POS_PARAM		_SET_Y(1, 1, 1)

#define ALT_UP(z)			_MV_Z(z, 0, 0)
#define ALT_DOWN(z)			_MV_Z(-z, 0, 0)
#define SET_ALT(z)			_SET_Z(z, 0, 0)
#define ALT_UP_PARAM		_MV_Z(1, 0, 1)
#define ALT_DOWN_PARAM		_MV_Z(-1, 0, 1)
#define SET_ALT_PARAM		_SET_Z(1, 0, 1)

#define SPEED_INCREASE(x)	_SPEED_INCREASE(x, 0)
#define SPEED_DECREASE(x)	_SPEED_INCREASE(-x, 0)
#define SET_SPEED(x)		_SET_SPEED(x, 0)
#define SPEED_INCREASE_PARAM _SPEED_INCREASE(1, 1)
#define SPEED_DECREASE_PARAM _SPEED_INCREASE(-1, 1)
#define SET_SPEED_PARAM		_SET_SPEED(0, 1)

#define FLAG_ON(f)			_FLAG_ON(f)
#define FLAG_OFF(f)			_FLAG_OFF(f)
#define FLAG_TOGGLE(f)		_FLAG_TOGGLE(f)

#define PEN_UP				_PEN_UP
#define PEN_DOWN			_PEN_DOWN
#define PEN_TOGGLE			_PEN_TOGGLE

#define SET_TURTLE(x)		_SET_TURTLE(x)

#define REPEAT(n)			_REPEAT(n, 0)
#define REPEAT_PARAM		_REPEAT(1, 1)
#define REPEAT_FOREVER		_REPEAT(-1, 0)
#define END					_END
#define ELSE				_ELSE

#define TO(func)			_TO(func)

#define DO(func)			_DO(func, 0, 0)
#define DO_ARG(func, arg)	_DO(func, arg, 0)
#define DO_PARAM(func)		_DO(func, 1, 1)

#define EXEC(func)			_EXEC(func, 0, 0)
#define EXEC_ARG(func, arg)	_EXEC(func, arg, 0)
#define EXEC_PARAM(func)	_EXEC(func, 1, 1)

#define PARAM_SET(x)		_PARAM_SET(x)
#define PARAM_ADD(x)		_PARAM_ADD(x)
#define PARAM_SUB(x)		_PARAM_ADD(-x)
#define PARAM_MUL(x)		_PARAM_MUL(x)
#define PARAM_DIV(x)		_PARAM_DIV(x)

#define SET_INTERRUPT(fn)	_SET_INTERRUPT(fn)
#define CLEAR_INTERRUPT		_CLEAR_INTERRUPT

#define LOAD_TO_PARAM(val)	_LOAD_TO_PARAM(val)

#define IF_EQ(val, x)		_IF_EQ(val, x, 0)
#define IF_NE(val, x)		_IF_NE(val, x, 0)
#define IF_GT(val, x)		_IF_GT(val, x, 0)
#define IF_LT(val, x)		_IF_LT(val, x, 0)
#define IF_GE(val, x)		_IF_GE(val, x, 0)
#define IF_LE(val, x)		_IF_LE(val, x, 0)
#define IF_EQ_PARAM(val)	_IF_EQ(val, 1, 1)
#define IF_NE_PARAM(val)	_IF_NE(val, 1, 1)
#define IF_GT_PARAM(val)	_IF_GT(val, 1, 1)
#define IF_LT_PARAM(val)	_IF_LT(val, 1, 1)
#define IF_GE_PARAM(val)	_IF_GE(val, 1, 1)
#define IF_LE_PARAM(val)	_IF_LE(val, 1, 1)

#define SET_POS(x, y)		_SET_X(x, 0, 0) _SET_Y(y, 1, 0)
#define SET_ABS_POS(x, y)	_SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF) _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF) \
							_SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF) _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)
#define HOME				_HOME(1)

*/
        private SerialPort serialPort = new SerialPort();
        private ClientTCP clientTCP = null;
        public StringBuilder debug = new StringBuilder("");
        private StringBuilder RxResponse = new StringBuilder("");
        private Boolean RxResponseIsNew = false;
        private int commTab = -1;
        private bool enhancedMode = false;
        private const int COMM_TAB_SERIAL = 0;
        private const int COMM_TAB_TCP = 1;

        public class logoInstructionDef
        {
            public byte cmd;
            public byte subcmd;
            public bool do_fly;
            public bool use_param;
            public short arg;

            public logoInstructionDef()
            {
                cmd = 0;
                do_fly = false;
                use_param = false;
                subcmd = 0;
                arg = 0;
            }
            public logoInstructionDef(logoInstructionDef that)
            {
                this.cmd = that.cmd;
                this.do_fly = that.do_fly;
                this.use_param = that.use_param;
                this.subcmd = that.subcmd;
                this.arg = that.arg;
            }
        }

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.Text += "  v" + Application.ProductVersion;
            Cmd_comboBox.SelectedIndex = 0;

            foreach (String str in Enum.GetNames(typeof(DataMsg.DataMsgCmd)))
            {
                if (str.Contains("Response"))
                    DataMsgCmd_comboBox.Items.Add("RESERVED");
                else
                    DataMsgCmd_comboBox.Items.Add(str.Replace("_", " "));
            }
            
            clientTCP = new ClientTCP(this);

            DiscoverSerialPorts();
            ServiceRegistry(false);

            if ((CommSerialPort_comboBox.Items.Count > 0) && (CommSerialPort_comboBox.SelectedIndex == -1))
                CommSerialPort_comboBox.SelectedIndex = 0;

            DataMsgType_AdvancedIP_radioButton_CheckedChanged(null, null); // set labels enabled
            Cmd_comboBox_SelectedIndexChanged(null, null);

            Housekeeping1sec_timer.Enabled = true;
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
            if (WriteSettings)
            {
                Application.UserAppDataRegistry.SetValue("ClientIP", ClientIP_textBox.Text);
                Application.UserAppDataRegistry.SetValue("Port", Port_numericUpDown.Value);
                Application.UserAppDataRegistry.SetValue("AutoConnect", Connect_checkBox.Checked);

                Application.UserAppDataRegistry.SetValue("Cmd", Cmd_comboBox.SelectedIndex);
                Application.UserAppDataRegistry.SetValue("SubCmd", SubCmd_numericUpDown.Value);
                Application.UserAppDataRegistry.SetValue("DoFly", DoFly_checkBox.Checked);
                Application.UserAppDataRegistry.SetValue("UseParam", UseParam_checkBox.Checked);
                Application.UserAppDataRegistry.SetValue("Arg", Arg_numericUpDown.Value);

                Application.UserAppDataRegistry.SetValue("CommSerialPort", CommSerialPort_comboBox.SelectedIndex);
                Application.UserAppDataRegistry.SetValue("CommSerialBaud", CommSerialBaud_comboBox.SelectedIndex);

                Application.UserAppDataRegistry.SetValue("DataMsgIP", DataMsgType_EnhancedLogo_radioButton.Checked);
                Application.UserAppDataRegistry.SetValue("DataMsgLegacy", DataMsgType_Legacy_radioButton.Checked);
                Application.UserAppDataRegistry.SetValue("DataMsgAutoIncCmd", AutoIncCmdIndex_checkBox.Checked);
                Application.UserAppDataRegistry.SetValue("DataMsgIndexMission", DataMsgType_IndexMission_numericUpDown.Value);
                Application.UserAppDataRegistry.SetValue("DataMsgIndexCmd", DataMsgType_IndexCmd_numericUpDown.Value);
                Application.UserAppDataRegistry.SetValue("DataMsgCmd", DataMsgCmd_comboBox.SelectedIndex);

                Application.UserAppDataRegistry.SetValue("TextWindowTab", TextWindow_tabControl.SelectedIndex);
                Application.UserAppDataRegistry.SetValue("ConnectionTab", ConnectionTab_tabControl.SelectedIndex);
                Application.UserAppDataRegistry.SetValue("ClearOnNewRx", AutoclearOnNewResponse_checkBox.Checked);
                Application.UserAppDataRegistry.SetValue("JumpToDebugOnNewMsg", JumpHereOnNewMsg_checkBox.Checked);
                
            }
            else
            {
                ClientIP_textBox.Text = Application.UserAppDataRegistry.GetValue("ClientIP", "uav").ToString();
                Port_numericUpDown.Value = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("Port", 3007));

                Cmd_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("Cmd", 0));
                SubCmd_numericUpDown.Value = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("SubCmd", 0));
                DoFly_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DoFly", false));
                UseParam_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("UseParam", false));
                Arg_numericUpDown.Value = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("Arg", 0));

                CommSerialBaud_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialBaud", 7));

                // protect against loading a COM port that no longer exists
                int comIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("CommSerialPort", -1));
                if (comIndex < CommSerialPort_comboBox.Items.Count)
                    CommSerialPort_comboBox.SelectedIndex = comIndex;
                
                DataMsgType_EnhancedLogo_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DataMsgIP", false));
                DataMsgType_Legacy_radioButton.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DataMsgLegacy", true));
                AutoIncCmdIndex_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("DataMsgAutoIncCmd", false));
                DataMsgType_IndexMission_numericUpDown.Value = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("DataMsgIndexMission", 0));
                DataMsgType_IndexCmd_numericUpDown.Value = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("DataMsgIndexCmd", 0));
                DataMsgCmd_comboBox.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("DataMsgCmd", 0));

                AutoclearOnNewResponse_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("ClearOnNewRx", false));
                JumpHereOnNewMsg_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("JumpToDebugOnNewMsg", true));
                ConnectionTab_tabControl.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("ConnectionTab", 0));
                TextWindow_tabControl.SelectedIndex = Convert.ToInt32(Application.UserAppDataRegistry.GetValue("TextWindowTab", 0));

                // always do this one last
                Connect_checkBox.Checked = Convert.ToBoolean(Application.UserAppDataRegistry.GetValue("AutoConnect", false));
            }

        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            ServiceRegistry(true);
            ClientDisconnect_button_Click(null, null);
        }
        public void Send(byte[] data)
        {
            clientTCP.Send(data);
        }

        private void ClientDisconnect_button_Click(object sender, EventArgs e)
        {
            Connect_checkBox.Checked = false;
            clientTCP.Disconnect();
            SerialDisconnect();
        }

        private void TextBoxClear_button_Click(object sender, EventArgs e)
        {
            debug_textBox.Text = "";
            CommandMessage_textBox.Text = "";
        }


        private void Connect_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            if (Connect_checkBox.Checked == false)
            {
                ClientDisconnect_button_Click(null, null);
                return;
            }
            if (ConnectionTab_tabControl.SelectedIndex == COMM_TAB_SERIAL)
            {
                SerialConnect();
            }
            else if (ConnectionTab_tabControl.SelectedIndex == COMM_TAB_TCP)
            {
                ushort port = Convert.ToUInt16(Port_numericUpDown.Value);
                clientTCP.Connect(ClientIP_textBox.Text, port);
            }
            else
            {
                debug.Append("\r\nWhat the heck, how did you connect without selecting Serial ro TCP?");
            }
        }
        private void UpdateIsConnected()
        {
            if ((ConnectionTab_tabControl.SelectedIndex == COMM_TAB_SERIAL) && (serialPort != null))
                IsConnected_radioButton.Checked = serialPort.IsOpen;
            else if ((ConnectionTab_tabControl.SelectedIndex == COMM_TAB_TCP) && (clientTCP != null))
                IsConnected_radioButton.Checked = clientTCP.isConnected();
            else
                IsConnected_radioButton.Checked = false;
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


        private void ClientIP_textBox_TextChanged(object sender, EventArgs e)
        {
            ClientDisconnect_button_Click(null, null);
        }

        private void IsConnected_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateIsConnected();
        }

        public void ParseRxPacket(byte[] packet)
        {
            if ((packet.Length >= 4) && // header length
                (packet[0] < Enum.GetNames(typeof(DataMsg.DataMsgCmd)).Length) && // Validate command
                (enhancedMode == true)) // eLogo expected
            {
                // appears to be a valid binary Logo command
                DataMsg.DataMsgCmd cmdType = (DataMsg.DataMsgCmd)packet[0];
                byte missionIndex = packet[1];
                byte cmdIndex = packet[2];
                byte instCount = packet[3];

                if ((cmdType == DataMsg.DataMsgCmd.Read_Mission_Response) ||
                    (cmdType == DataMsg.DataMsgCmd.Read_Cmd_Response))
                {
                    RxResponseIsNew = true;
                    for (int i = 0; i < instCount; i++)
                    {
                        logoInstructionDef instr = DataMsg.ConvertPacktToInst(packet, 4 + i*6);
                        String str = RxLogoParser.ConvertInstrToString(instr);
                        RxResponse.Append(str);
                    }
                }
            }
            else
            {
                // Just assume ascii and dump to the screen
                ASCIIEncoding encoder = new ASCIIEncoding();
                RxResponse.Append(encoder.GetString(packet, 0, packet.Length));
            }
        }

        private void Send_button_Click(object sender, EventArgs e)
        {
            byte[] packet;
            // If we're not already connected, make it so.
            Connect_checkBox.Checked = true;

            if ((ConnectionTab_tabControl.SelectedIndex == COMM_TAB_SERIAL))
            {
                packet = System.Text.Encoding.ASCII.GetBytes(CommandMessage_textBox.Text);
                if (serialPort.IsOpen)
                {
                    try
                    {
                        serialPort.Write(packet, 0, packet.Length);
                    }
                    catch
                    {
                        debug.Append("error while trying to write to ther serial port");
                        //ClientDisconnect_button_Click(null, null);
                    }
                }
            }
            else if (ConnectionTab_tabControl.SelectedIndex == COMM_TAB_TCP)
            {
                if (DataMsgType_EnhancedLogo_radioButton.Checked)
                    packet = generateDataMsgPacket();
                else
                    packet = System.Text.Encoding.ASCII.GetBytes(CommandMessage_textBox.Text);

                bool success = clientTCP.Send(packet);
                if (success && AutoIncCmdIndex_checkBox.Checked)
                {
                    DataMsgType_IndexCmd_numericUpDown.Value++;
                }
            }
            else
            {
                debug.Append("\r\nHow the heck did you try to send while not in Serial or TCP mode?");
            }
        }
        private void HandleMsgChange(bool setLabels)
        {
            logoInstructionDef msg = CreateMsgData();
            if (setLabels)
            {
                SetLabels(msg);
            }
            if ((ConnectionTab_tabControl.SelectedIndex == COMM_TAB_TCP) &&
                (DataMsgType_EnhancedLogo_radioButton.Checked == true))
            {
                CommandMessage_textBox.Text = "";
            }
            else
            {
                CommandMessage_textBox.Text = ConvertMsgToString(msg);
            }
        }
        private void Cmd_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            HandleMsgChange(true);
        }
        private void SubCmd_numericUpDown_ValueChanged(object sender, EventArgs e)
        {
            HandleMsgChange(false);
        }
        private void DoFly_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            HandleMsgChange(false);
        }
        private void UseParam_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            HandleMsgChange(false);
        }
        private void Arg_numericUpDown_ValueChanged(object sender, EventArgs e)
        {
            HandleMsgChange(false);
        }

        private logoInstructionDef CreateMsgData()
        {
            logoInstructionDef msg = new logoInstructionDef();

            msg.subcmd = Convert.ToByte(SubCmd_numericUpDown.Value);
            msg.do_fly = DoFly_checkBox.Checked;
            msg.use_param = UseParam_checkBox.Checked;
            msg.arg = Convert.ToInt16(Arg_numericUpDown.Value);

            switch (Cmd_comboBox.SelectedIndex)
            {           //          		        cmd,fly,param,sub,x
                case 0: // _REPEAT(n, pr)		    {1,	0,	pr,	0,	n},
                case 1: // _END					    {1,	0,	0,	1,	0},
                case 2: // _ELSE					{1,	0,	0,	3,	0},
                case 3: // _TO(fn)				    {1,	0,	0,	2,	fn},
                case 88: // REPEAT(n)			    _REPEAT(n, 0)
                case 89: // REPEAT_PARAM		    _REPEAT(1, 1)
                case 90: // REPEAT_FOREVER		    _REPEAT(-1, 0)
                case 91: // END					    _END
                case 92: // ELSE				    _ELSE
                case 93: // TO(func)			    _TO(func)
                    msg.cmd = 1;
                    break;                

                case 4: // _DO(fn, x, pr)		    {2, 0,	pr,	fn, x},
                case 94: // DO(func)			    _DO(func, 0, 0)
                case 95: // DO_ARG(func, arg)	    _DO(func, arg, 0)
                case 96: // DO_PARAM(func)		    _DO(func, 1, 1)
                    msg.cmd = 2;
                    break;

                case 5: // _EXEC(fn, x, pr)		    {10,0,	pr,	fn, x},
                case 97: // EXEC(func)			    _EXEC(func, 0, 0)
                case 98: // EXEC_ARG(func, arg)	    _EXEC(func, arg, 0)
                case 99: // EXEC_PARAM(func)	    _EXEC(func, 1, 1)
                    msg.cmd = 10;
                    break;

                case 6: // _FD(x, fl, pr)		    {3,	fl,	pr,	0,	x},
                case 44: // FD(x)				    _FD(x, 1, 0)
                case 45: // BK(x)				    _FD(-x, 1, 0)
                case 46: // FD_PARAM			    _FD(1, 1, 1)
                case 47: // BK_PARAM			    _FD(-1, 1, 1)
                    msg.cmd = 3;
                    break;

                case 7: // _RT(x, pr)			    {4,	0,	pr, 0,	x},
                case 8: // _SET_ANGLE(x, pr)		{4,	0,	pr, 1,	x},
                case 9: // _USE_CURRENT_ANGLE	    {4,	0,	0,	2,	0},
                case 10: // _USE_ANGLE_TO_GOAL	    {4,	0,	0,	3,	0},
                case 48: // RT(x)				    _RT(x, 0)
                case 49: // LT(X)				    _RT(-x, 0)
                case 50: // SET_ANGLE(x)		    _SET_ANGLE(x, 0)
                case 51: // RT_PARAM			    _RT(1, 1)
                case 52: // LT_PARAM			    _RT(-1, 1)
                case 53: // SET_ANGLE_PARAM		    _SET_ANGLE(0, 1)
                case 54: // USE_CURRENT_ANGLE	    _USE_CURRENT_ANGLE
                case 55: // USE_ANGLE_GOAL	        _USE_ANGLE_TO_GOAL
                    msg.cmd = 4;
                    break;

                case 11: // _MV_X(x, fl, pr)		{5,	fl,	pr,	0,	x},
                case 12: // _SET_X(x, fl, pr)		{5,	fl,	pr,	1,	x},
                case 13: // _MV_Y(y, fl, pr)		{5,	fl,	pr,	2,	y},
                case 14: // _SET_Y(y, fl, pr)		{5,	fl,	pr,	3,	y},
                case 15: // _MV_Z(z, fl, pr)		{5,	fl,	pr,	4,	z},
                case 16: // _SET_Z(z, fl, pr)		{5,	fl,	pr,	5,	z},
                case 17: // _USE_CURRENT_POS(fl)	{5, fl,	0,	6,	0},
                case 18: // _HOME(fl)				{5,	fl,	0,	7,	0},
                case 19: // _SET_ABS_VAL_HIGH(x)	{5,	0,	0,	8,	x}, // Set the high and then low words for X and
                case 20: // _SET_ABS_X_LOW(x)		{5,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
                case 21: // _SET_ABS_Y_LOW(y, fl)	{5,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)
                case 56: // EAST(x)				    _MV_X(x, 1, 0)
                case 57: // WEST(x)				    _MV_X(-x, 1, 0)
                case 58: // SET_X_POS(x)		    _SET_X(x, 1, 0)
                case 59: // EAST_PARAM			    _MV_X(1, 1, 1)
                case 60: // WEST_PARAM			    _MV_X(-1, 1, 1)
                case 61: // SET_X_POS_PARAM		    _SET_X(1, 1, 1)
                case 62: // USE_CURRENT_POS		    _USE_CURRENT_POS(1)
                case 63: // NORTH(y)			    _MV_Y(y, 1, 0)
                case 64: // SOUTH(y)			    _MV_Y(-y, 1, 0)
                case 65: // SET_Y_POS(y)		    _SET_Y(y, 1, 0)
                case 66: // NORTH_PARAM		        _MV_Y(1, 1, 1)
                case 67: // SOUTH_PARAM			    _MV_Y(-1, 1, 1)
                case 68: // SET_Y_POS_PARAM	        _SET_Y(1, 1, 1)
                case 69: // ALT_UP(z)			    _MV_Z(z, 0, 0)
                case 70: // ALT_DOWN(z)			    _MV_Z(-z, 0, 0)
                case 71: // SET_ALT(z)			    _SET_Z(z, 0, 0)
                case 72: // ALT_UP_PARAM		    _MV_Z(1, 0, 1)
                case 73: // ALT_DOWN_PARAM		    _MV_Z(-1, 0, 1)
                case 74: // SET_ALT_PARAM		    _SET_Z(1, 0, 1)
                case 120: // SET_POS(x, y)		    _SET_X(x, 0, 0) _SET_Y(y, 1, 0)
                case 121: // SET_ABS_POS(x, y)	    _SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF)  _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF) 	_SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF)  _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)
                case 122: // HOME				    _HOME(1)
                    msg.cmd = 5;
                    break;

                case 22: // _FLAG_ON(f)			    {6,	0,	0,	0,	f},
                case 23: // _FLAG_OFF(f)			{6,	0,	0,	1,	f},
                case 24: // _FLAG_TOGGLE(f)		    {6,	0,	0,	2,	f},
                case 81: // FLAG_ON(f)		        _FLAG_ON(f)
                case 82: // FLAG_OFF(f)			    _FLAG_OFF(f)
                case 83: // FLAG_TOGGLE(f)		    _FLAG_TOGGLE(f)
                    msg.cmd = 6;
                    break;

                case 25: // _PEN_UP				    {7,	0,	0,	0,	0},
                case 26: // _PEN_DOWN				{7,	1,	0,	1,	0},
                case 27: // _PEN_TOGGLE			    {7,	0,	0,	2,	0},
                case 84: // PEN_UP				    _PEN_UP
                case 85: // PEN_DOWN			    _PEN_DOWN
                case 86: // PEN_TOGGLE			    _PEN_TOGGLE
                    msg.cmd = 7;
                    break;

                case 28: // _SET_TURTLE(x)		    {8,	0,	0,	0,	x},
                case 87: // SET_TURTLE(x)		    _SET_TURTLE(x)
                    msg.cmd = 8;
                    break;

                case 29: // _PARAM_SET(x)			{9,	0,	0,	0,	x},
                case 30: // _PARAM_ADD(x)			{9,	0,	0,	1,	x},
                case 31: // _PARAM_MUL(x)			{9,	0,	0,	2,	x},
                case 32: // _PARAM_DIV(x)			{9,	0,	0,	3,	x},
                case 100: // PARAM_SET(x)		    _PARAM_SET(x)
                case 101: // PARAM_ADD(x)		    _PARAM_ADD(x)
                case 102: // PARAM_SUB(x)		    _PARAM_ADD(-x)
                case 103: // PARAM_MUL(x)		    _PARAM_MUL(x)
                case 104: // PARAM_DIV(x)		    _PARAM_DIV(x)
                    msg.cmd = 9;
                    break;

                case 33: // _SPEED_INCREASE(s, pr)  {11,0,	pr,	0,	s},
                case 34: // _SET_SPEED(s, pr)		{11,0,	pr,	1,	s},
                case 75: // SPEED_INCREASE(x)	    _SPEED_INCREASE(x, 0)
                case 76: // SPEED_DECREASE(x)	    _SPEED_INCREASE(-x, 0)
                case 77: // SET_SPEED(x)		    _SET_SPEED(x, 0)
                case 78: // SPEED_INCREASE_PARAM    _SPEED_INCREASE(1, 1)
                case 79: // SPEED_DECREASE_PARAM    _SPEED_INCREASE(-1, 1)
                case 80: // SET_SPEED_PARAM	        _SET_SPEED(0, 1)
                    msg.cmd = 11;
                    break;

                case 35: // _SET_INTERRUPT(fn)	    {12,0,	0,	1,	fn},
                case 36: // _CLEAR_INTERRUPT		{12,0,	0,	0,	0},
                case 105: // SET_INTERRUPT(fn)	    _SET_INTERRUPT(fn)
                case 106: // CLEAR_INTERRUPT		_CLEAR_INTERRUPT
                    msg.cmd = 12;
                    break;

                case 37: // _LOAD_TO_PARAM(val)	    {13,0,	0,	val,0},
                case 107: // LOAD_TO_PARAM(val)	    _LOAD_TO_PARAM(val)
                    msg.cmd = 13;
                    break;

                case 38: // _IF_EQ(val, x, pr)	    {14,0,	pr,	val,x},
                case 108: // IF_EQ(val, x)		    _IF_EQ(val, x, 0)
                case 114: // IF_EQ_PARAM(val)	    _IF_EQ(val, 1, 1)
                    msg.cmd = 14;
                    break;

                case 39: // _IF_NE(val, x, pr)	    {15,0,	pr,	val,x},
                case 109: // IF_NE(val, x)		    _IF_NE(val, x, 0)
                case 115: // IF_NE_PARAM(val)	    _IF_NE(val, 1, 1)
                    msg.cmd = 15;
                    break;

                case 40: // _IF_GT(val, x, pr)	    {16,0,	pr,	val,x},
                case 110: // IF_GT(val, x)		    _IF_GT(val, x, 0)
                case 116: // IF_GT_PARAM(val)	    _IF_GT(val, 1, 1)
                    msg.cmd = 16;
                    break;

                case 41: // _IF_LT(val, x, pr)	    {17,0,	pr,	val,x},
                case 111: // IF_LT(val, x)		    _IF_LT(val, x, 0)
                case 117: // IF_LT_PARAM(val)	    _IF_LT(val, 1, 1)
                    msg.cmd = 17;
                    break;

                case 42: // _IF_GE(val, x, pr)	    {18,0,	pr,	val,x},
                case 112: // IF_GE(val, x)		    _IF_GE(val, x, 0)
                case 118: // IF_GE_PARAM(val)	    _IF_GE(val, 1, 1)
                    msg.cmd = 18;
                    break;

                case 43: // _IF_LE(val, x, pr)	    {19,0,	pr,	val,x},
                case 113: // IF_LE(val, x)		    _IF_LE(val, x, 0)
                case 119: // IF_LE_PARAM(val)	    _IF_LE(val, 1, 1)
                    msg.cmd = 19;
                    break;

                default:
                    msg.cmd = 0; // invalid?
                    break;
            }
            return msg;
        }

       
        private string ConvertMsgToString(logoInstructionDef msg)
        {
            // For UDB Logo instructions, bytes should be passed in using the following format
            // (Below, an X represents a hex digit 0-F.  Mulit-digit values are MSB first.)
            // L			begin remote Logo command
            // XX	byte:	command
            // XX	byte:	subcommand
            // X	0-1:	do fly
            // X	0-1:	use param
            // XXXX	word:	argument
            // *			done with command data
            // XX	byte:	checksum should equal the sum of the 10 bytes before the *, mod 256
            // 
            // For example: "L0201000005*E8" runs:
            // the DO command(02) for subroutine 01 with fly and param off(00) and an argument of 0005


            // data
            StringBuilder hex = new StringBuilder();
            hex.AppendFormat("{0:X2}", msg.cmd);
            hex.AppendFormat("{0:X2}", msg.subcmd);
            hex.AppendFormat("{0:X1}", Convert.ToByte(msg.do_fly));
            hex.AppendFormat("{0:X1}", Convert.ToByte(msg.use_param));
            hex.AppendFormat("{0:X4}", msg.arg);

            // checksum
            byte[] ascii = System.Text.Encoding.ASCII.GetBytes(hex.ToString());
            byte checksum = 0;
            for (int i = 0; i < ascii.Length; i++)
            {
                checksum += ascii[i];
            }
            hex.Append("*");
            hex.AppendFormat("{0:X2}", checksum);

            // Add the header "L" and we're done!
            return "L" + hex.ToString();
        }

        private void SetLabels(logoInstructionDef msg)
        {
            DoFly_label.Visible = false;
            UseParam_label.Visible = false;
            SubCmd_label.Visible = false;
            Argument_label.Visible = false;

            switch (Cmd_comboBox.SelectedIndex)
            {
                        //            		        cmd,fly,param,sub,x
                case 0: // _REPEAT(n, pr)		    {1,	0,	pr,	0,	n},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( n )";
                    break;

                case 1: // _END					    {1,	0,	0,	1,	0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Arg_numericUpDown.Value = 0;
                    break;

                case 2: // _ELSE					{1,	0,	0,	3,	0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 3;
                    Arg_numericUpDown.Value = 0;
                    break;

                case 3: // _TO(fn)				    {1,	0,	0,	2,	fn},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 2;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( fn )";
                    break;

                case 4: // _DO(fn, x, pr)		    {2, 0,	pr,	fn, x},
                case 5: // _EXEC(fn, x, pr)		    {10,0,	pr,	fn, x},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_label.Visible = true; SubCmd_label.Text = "< --- ( fn )";
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 6: // _FD(x, fl, pr)		    {3,	fl,	pr,	0,	x},
                case 11: // _MV_X(x, fl, pr)		{5,	fl,	pr,	0,	x},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 2;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 7: // _RT(x, pr)			    {4,	0,	pr, 0,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 8: // _SET_ANGLE(x, pr)		{4,	0,	pr, 1,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 9: // _USE_CURRENT_ANGLE	    {4,	0,	0,	2,	0},
                case 54: // USE_CURRENT_ANGLE	    _USE_CURRENT_ANGLE
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 2;
                    Arg_numericUpDown.Value = 0;
                    break;

                case 10: // _USE_ANGLE_TO_GOAL	    {4,	0,	0,	3,	0},
                case 55: // USE_ANGLE_GOAL	        _USE_ANGLE_TO_GOAL
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 3;
                    Arg_numericUpDown.Value = 0;
                    break;

                case 12: // _SET_X(x, fl, pr)		{5,	fl,	pr,	1,	x},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 13: // _MV_Y(y, fl, pr)		{5,	fl,	pr,	2,	y},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 2;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( y )";
                    break;
                case 14: // _SET_Y(y, fl, pr)		{5,	fl,	pr,	3,	y},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 3;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 15: // _MV_Z(z, fl, pr)		{5,	fl,	pr,	4,	z},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 4;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( z )";
                    break;
                case 16: // _SET_Z(z, fl, pr)		{5,	fl,	pr,	5,	z},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 5;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( z )";
                    break;
                case 17: // _USE_CURRENT_POS(fl)	{5, fl,	0,	6,	0},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 6;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 18: // _HOME(fl)				{5,	fl,	0,	7,	0},
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 7;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 19: // _SET_ABS_VAL_HIGH(x)	{5,	0,	0,	8,	x}, // Set the high and then low words for X and
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 8;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 20: // _SET_ABS_X_LOW(x)		{5,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 9;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 21: // _SET_ABS_Y_LOW(y, fl)	{5,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)
                    DoFly_label.Visible = false; DoFly_label.Text = "< --- ( fl )";
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 10;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( y )";
                    break;
                case 22: // _FLAG_ON(f)			    {6,	0,	0,	0,	f},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( f )";
                    break;
                case 23: // _FLAG_OFF(f)			{6,	0,	0,	1,	f},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( f )";
                    break;
                case 24: // _FLAG_TOGGLE(f)		    {6,	0,	0,	2,	f},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 2;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( f )";
                    break;
                case 25: // _PEN_UP				    {7,	0,	0,	0,	0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 26: // _PEN_DOWN				{7,	1,	0,	1,	0},
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 27: // _PEN_TOGGLE			    {7,	0,	0,	2,	0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 2;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 28: // _SET_TURTLE(x)		    {8,	0,	0,	0,	x},
                case 29: // _PARAM_SET(x)			{9,	0,	0,	0,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 30: // _PARAM_ADD(x)			{9,	0,	0,	1,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 31: // _PARAM_MUL(x)			{9,	0,	0,	2,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 32: // _PARAM_DIV(x)			{9,	0,	0,	3,	x},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 3;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 33: // _SPEED_INCREASE(s, pr)  {11,0,	pr,	0,	s},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( s )";
                    break;
                case 34: // _SET_SPEED(s, pr)		{11,0,	pr,	1,	s},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( s )";
                    break;
                case 35: // _SET_INTERRUPT(fn)	    {12,0,	0,	1,	fn},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( fn )";
                    break;
                case 36: // _CLEAR_INTERRUPT		{12,0,	0,	0,	0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = 0;
                    break;
                case 37: // _LOAD_TO_PARAM(val)	    {13,0,	0,	val,0},
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    SubCmd_label.Visible = true; SubCmd_label.Text = "< --- ( val )";
                    Arg_numericUpDown.Value = 0;
                    break;
                case 38: // _IF_EQ(val, x, pr)	    {14,0,	pr,	val,x},
                case 39: // _IF_NE(val, x, pr)	    {15,0,	pr,	val,x},
                case 40: // _IF_GT(val, x, pr)	    {16,0,	pr,	val,x},
                case 41: // _IF_LT(val, x, pr)	    {17,0,	pr,	val,x},
                case 42: // _IF_GE(val, x, pr)	    {18,0,	pr,	val,x},
                case 43: // _IF_LE(val, x, pr)	    {19,0,	pr,	val,x},
                    DoFly_checkBox.Checked = false;
                    UseParam_label.Visible = true; UseParam_label.Text = "< --- ( pr )";
                    SubCmd_label.Visible = true; SubCmd_label.Text = "< --- ( val )";
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 44: // FD(x)				    _FD(x, 1, 0)
                case 45: // BK(x)				    _FD(-x, 1, 0)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 46: // FD_PARAM			    _FD(1, 1, 1)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = true;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = 1;
                    break;
                case 47: // BK_PARAM			    _FD(-1, 1, 1)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = true;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = -1;
                    break;

                case 48: // RT(x)				    _RT(x, 0)
                case 49: // LT(X)				    _RT(-x, 0)
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 50: // SET_ANGLE(x)		    _SET_ANGLE(x, 0)
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 51: // RT_PARAM			    _RT(1, 1)
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = true;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = 1;
                    break;
                case 52: // LT_PARAM			    _RT(-1, 1)
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = true;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = -1;
                    break;
                case 53: // SET_ANGLE_PARAM		    _SET_ANGLE(0, 1)
                    DoFly_checkBox.Checked = false;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Arg_numericUpDown.Value = 0;
                    break;

                case 56: // EAST(x)				    _MV_X(x, 1, 0)
                case 57: // WEST(x)				    _MV_X(-x, 1, 0)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;
                case 58: // SET_X_POS(x)		    _SET_X(x, 1, 0)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 1;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 59: // EAST_PARAM			    _MV_X(1, 1, 1)
                case 60: // WEST_PARAM			    _MV_X(-1, 1, 1)
                    DoFly_checkBox.Checked = true;
                    UseParam_checkBox.Checked = false;
                    SubCmd_numericUpDown.Value = 0;
                    Argument_label.Visible = true; Argument_label.Text = "< --- ( x )";
                    break;

                case 61: // SET_X_POS_PARAM		    _SET_X(1, 1, 1)
                case 62: // USE_CURRENT_POS		    _USE_CURRENT_POS(1)
                case 63: // NORTH(y)			    _MV_Y(y, 1, 0)
                case 64: // SOUTH(y)			    _MV_Y(-y, 1, 0)
                case 65: // SET_Y_POS(y)		    _SET_Y(y, 1, 0)
                case 66: // NORTH_PARAM		        _MV_Y(1, 1, 1)
                case 67: // SOUTH_PARAM			    _MV_Y(-1, 1, 1)
                case 68: // SET_Y_POS_PARAM	        _SET_Y(1, 1, 1)
                case 69: // ALT_UP(z)			    _MV_Z(z, 0, 0)
                case 70: // ALT_DOWN(z)			    _MV_Z(-z, 0, 0)
                case 71: // SET_ALT(z)			    _SET_Z(z, 0, 0)
                case 72: // ALT_UP_PARAM		    _MV_Z(1, 0, 1)
                case 73: // ALT_DOWN_PARAM		    _MV_Z(-1, 0, 1)
                case 74: // SET_ALT_PARAM		    _SET_Z(1, 0, 1)
                case 75: // SPEED_INCREASE(x)	    _SPEED_INCREASE(x, 0)
                case 76: // SPEED_DECREASE(x)	    _SPEED_INCREASE(-x, 0)
                case 77: // SET_SPEED(x)		    _SET_SPEED(x, 0)
                case 78: // SPEED_INCREASE_PARAM    _SPEED_INCREASE(1, 1)
                case 79: // SPEED_DECREASE_PARAM    _SPEED_INCREASE(-1, 1)
                case 80: // SET_SPEED_PARAM	        _SET_SPEED(0, 1)
                case 81: // FLAG_ON(f)		        _FLAG_ON(f)
                case 82: // FLAG_OFF(f)			    _FLAG_OFF(f)
                case 83: // FLAG_TOGGLE(f)		    _FLAG_TOGGLE(f)
                case 84: // PEN_UP				    _PEN_UP
                case 85: // PEN_DOWN			    _PEN_DOWN
                case 86: // PEN_TOGGLE			    _PEN_TOGGLE
                case 87: // SET_TURTLE(x)		    _SET_TURTLE(x)
                case 88: // REPEAT(n)			    _REPEAT(n, 0)
                case 89: // REPEAT_PARAM		    _REPEAT(1, 1)
                case 90: // REPEAT_FOREVER		    _REPEAT(-1, 0)
                case 91: // END					    _END
                case 92: // ELSE				    _ELSE
                case 93: // TO(func)			    _TO(func)
                case 94: // DO(func)			    _DO(func, 0, 0)
                case 95: // DO_ARG(func, arg)	    _DO(func, arg, 0)
                case 96: // DO_PARAM(func)		    _DO(func, 1, 1)
                case 97: // EXEC(func)			    _EXEC(func, 0, 0)
                case 98: // EXEC_ARG(func, arg)	    _EXEC(func, arg, 0)
                case 99: // EXEC_PARAM(func)	    _EXEC(func, 1, 1)
                case 100: // PARAM_SET(x)		    _PARAM_SET(x)
                case 101: // PARAM_ADD(x)		    _PARAM_ADD(x)
                case 102: // PARAM_SUB(x)		    _PARAM_ADD(-x)
                case 103: // PARAM_MUL(x)		    _PARAM_MUL(x)
                case 104: // PARAM_DIV(x)		    _PARAM_DIV(x)
                case 105: // SET_INTERRUPT(fn)	    _SET_INTERRUPT(fn)
                case 106: // CLEAR_INTERRUPT		_CLEAR_INTERRUPT
                case 107: // LOAD_TO_PARAM(val)	    _LOAD_TO_PARAM(val)
                case 108: // IF_EQ(val, x)		    _IF_EQ(val, x, 0)
                case 109: // IF_NE(val, x)		    _IF_NE(val, x, 0)
                case 110: // IF_GT(val, x)		    _IF_GT(val, x, 0)
                case 111: // IF_LT(val, x)		    _IF_LT(val, x, 0)
                case 112: // IF_GE(val, x)		    _IF_GE(val, x, 0)
                case 113: // IF_LE(val, x)		    _IF_LE(val, x, 0)
                case 114: // IF_EQ_PARAM(val)	    _IF_EQ(val, 1, 1)
                case 115: // IF_NE_PARAM(val)	    _IF_NE(val, 1, 1)
                case 116: // IF_GT_PARAM(val)	    _IF_GT(val, 1, 1)
                case 117: // IF_LT_PARAM(val)	    _IF_LT(val, 1, 1)
                case 118: // IF_GE_PARAM(val)	    _IF_GE(val, 1, 1)
                case 119: // IF_LE_PARAM(val)	    _IF_LE(val, 1, 1)
                case 120: // SET_POS(x, y)		    _SET_X(x, 0, 0) _SET_Y(y, 1, 0)
                case 121: // SET_ABS_POS(x, y)	    _SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF)  _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF) 	_SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF)  _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)
                case 122: // HOME				    _HOME(1)
                    break;
            
            } // switch
        }

        private void Housekeeping1sec_timer_Tick(object sender, EventArgs e)
        {
            UpdateIsConnected();
        }

        private void DataMsgType_AdvancedIP_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            bool enabled = DataMsgType_EnhancedLogo_radioButton.Checked;
            DataMsgType_IndexMission_numericUpDown.Enabled = enabled;
            DataMsgType_IndexCmd_numericUpDown.Enabled = enabled;
            DataMsgCmd_comboBox.Enabled = enabled;
            enhancedMode = enabled;

            label8.Enabled = enabled;
            label10.Enabled = enabled;
        }

        private byte[] generateDataMsgPacket()
        {
            // only 1 command is supported right now by this
            // app, but you can send up to logoInstructionDef[x]
            logoInstructionDef[] inst = new logoInstructionDef[1];

            inst[0] = new logoInstructionDef(CreateMsgData());

            DataMsg dataMsg = new DataMsg(DataMsgCmd_comboBox.SelectedIndex, 
                (byte)DataMsgType_IndexMission_numericUpDown.Value,
                (byte)DataMsgType_IndexCmd_numericUpDown.Value);

            return DataMsg.generatepacket(dataMsg, inst);
        }

        private void Housekeeping100ms_timer_Tick(object sender, EventArgs e)
        {
            if (debug.Length > 0)
            {
                debug_textBox.AppendText(debug.ToString());
                debug.Length = 0; // google says ".Length=0" is 25% faster than "new StringBuilder()"
                if (JumpHereOnNewMsg_checkBox.Checked)
                    TextWindow_tabControl.SelectedIndex = 1; // debug
            }
            if (RxResponse.Length > 0)
            {
                if (AutoclearOnNewResponse_checkBox.Checked && RxResponseIsNew)
                {
                    RxResponseIsNew = false;
                    RxResponse_textBox.Text = RxResponse.ToString();
                }
                else
                {
                    RxResponse_textBox.AppendText(RxResponse.ToString());
                }

                RxResponse.Length = 0;
            }
        }

        private void ClearDebug_button_Click(object sender, EventArgs e)
        {
            debug_textBox.Text = "";
        }

        private void ClearRxResponse_button_Click(object sender, EventArgs e)
        {
            RxResponse_textBox.Text = "";
        }

        private void ConnectionTab_tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            commTab = ConnectionTab_tabControl.SelectedIndex;
            Cmd_comboBox_SelectedIndexChanged(null, null);
        }



    } // class
} // namespace
