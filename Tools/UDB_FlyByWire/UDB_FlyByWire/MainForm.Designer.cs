namespace UDB_FlyByWire
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.timer_data = new System.Windows.Forms.Timer(this.components);
            this.timer_status = new System.Windows.Forms.Timer(this.components);
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.joystickComboBox = new System.Windows.Forms.ComboBox();
            this.Joystick_timer = new System.Windows.Forms.Timer(this.components);
            this.IsJoyFound_label = new System.Windows.Forms.Label();
            this.port_label = new System.Windows.Forms.Label();
            this.EnableTCPServer_checkBox = new System.Windows.Forms.CheckBox();
            this.Mode_comboBox = new System.Windows.Forms.ComboBox();
            this.TCPDebug_checkBox = new System.Windows.Forms.CheckBox();
            this.InvertX_checkBox = new System.Windows.Forms.CheckBox();
            this.InvertY_checkBox = new System.Windows.Forms.CheckBox();
            this.InvertZ_checkBox = new System.Windows.Forms.CheckBox();
            this.JoyIP_checkBox = new System.Windows.Forms.CheckBox();
            this.Aileron_trackBar = new System.Windows.Forms.TrackBar();
            this.Elevator_trackBar = new System.Windows.Forms.TrackBar();
            this.Rudder_trackBar = new System.Windows.Forms.TrackBar();
            this.OverrideJoy_checkBox = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.CopyXtoZ_checkBox = new System.Windows.Forms.CheckBox();
            this.DynDNS_textBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.Port_textBox = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.JoyCfg_tabPage = new System.Windows.Forms.TabPage();
            this.CopyYtoZ_checkBox = new System.Windows.Forms.CheckBox();
            this.CopyXtoY_checkBox = new System.Windows.Forms.CheckBox();
            this.JoyData_tabPage = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.Throttle_trackBar = new System.Windows.Forms.TrackBar();
            this.JoyReset_button = new System.Windows.Forms.Button();
            this.Connection_tabPage = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ModeServer_radioButton = new System.Windows.Forms.RadioButton();
            this.ModeClient_radioButton = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.IpTypeUDP_radioButton = new System.Windows.Forms.RadioButton();
            this.IpTypeTCP_radioButton = new System.Windows.Forms.RadioButton();
            this.Debug_tabPage = new System.Windows.Forms.TabPage();
            this.JoyStickDebug_checkBox = new System.Windows.Forms.CheckBox();
            this.TextBoxClear_button = new System.Windows.Forms.Button();
            this.About_tabPage = new System.Windows.Forms.TabPage();
            this.HouseKeeping_1sec_timer = new System.Windows.Forms.Timer(this.components);
            this.DataRx_label = new System.Windows.Forms.Label();
            this.EmailTomPittenger_button = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.Aileron_trackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Elevator_trackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Rudder_trackBar)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.JoyCfg_tabPage.SuspendLayout();
            this.JoyData_tabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Throttle_trackBar)).BeginInit();
            this.Connection_tabPage.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.Debug_tabPage.SuspendLayout();
            this.About_tabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.Location = new System.Drawing.Point(3, 6);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(309, 320);
            this.textBox1.TabIndex = 0;
            // 
            // timer_data
            // 
            this.timer_data.Enabled = true;
            this.timer_data.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timer_status
            // 
            this.timer_status.Enabled = true;
            this.timer_status.Interval = 1000;
            this.timer_status.Tick += new System.EventHandler(this.timer_status_Tick);
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Location = new System.Drawing.Point(111, 205);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(77, 17);
            this.radioButton1.TabIndex = 2;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "Connected";
            this.radioButton1.UseVisualStyleBackColor = true;
            // 
            // joystickComboBox
            // 
            this.joystickComboBox.FormattingEnabled = true;
            this.joystickComboBox.Location = new System.Drawing.Point(21, 55);
            this.joystickComboBox.Name = "joystickComboBox";
            this.joystickComboBox.Size = new System.Drawing.Size(207, 21);
            this.joystickComboBox.TabIndex = 4;
            // 
            // Joystick_timer
            // 
            this.Joystick_timer.Enabled = true;
            this.Joystick_timer.Tick += new System.EventHandler(this.Joystick_timer_Tick);
            // 
            // IsJoyFound_label
            // 
            this.IsJoyFound_label.AutoSize = true;
            this.IsJoyFound_label.Location = new System.Drawing.Point(133, 39);
            this.IsJoyFound_label.Name = "IsJoyFound_label";
            this.IsJoyFound_label.Size = new System.Drawing.Size(95, 13);
            this.IsJoyFound_label.TabIndex = 8;
            this.IsJoyFound_label.Text = "Is Joystick Found?";
            // 
            // port_label
            // 
            this.port_label.AutoSize = true;
            this.port_label.Location = new System.Drawing.Point(194, 207);
            this.port_label.Name = "port_label";
            this.port_label.Size = new System.Drawing.Size(25, 13);
            this.port_label.TabIndex = 9;
            this.port_label.Text = "port";
            // 
            // EnableTCPServer_checkBox
            // 
            this.EnableTCPServer_checkBox.AutoSize = true;
            this.EnableTCPServer_checkBox.Location = new System.Drawing.Point(56, 136);
            this.EnableTCPServer_checkBox.Name = "EnableTCPServer_checkBox";
            this.EnableTCPServer_checkBox.Size = new System.Drawing.Size(117, 17);
            this.EnableTCPServer_checkBox.TabIndex = 10;
            this.EnableTCPServer_checkBox.Text = "Enable TCP Server";
            this.EnableTCPServer_checkBox.UseVisualStyleBackColor = true;
            this.EnableTCPServer_checkBox.CheckedChanged += new System.EventHandler(this.EnableServer_checkBox_CheckedChanged);
            // 
            // Mode_comboBox
            // 
            this.Mode_comboBox.FormattingEnabled = true;
            this.Mode_comboBox.Items.AddRange(new object[] {
            "Manual",
            "Stabilized",
            "Waypoint"});
            this.Mode_comboBox.Location = new System.Drawing.Point(111, 288);
            this.Mode_comboBox.Name = "Mode_comboBox";
            this.Mode_comboBox.Size = new System.Drawing.Size(115, 21);
            this.Mode_comboBox.TabIndex = 16;
            // 
            // TCPDebug_checkBox
            // 
            this.TCPDebug_checkBox.AutoSize = true;
            this.TCPDebug_checkBox.Location = new System.Drawing.Point(6, 332);
            this.TCPDebug_checkBox.Name = "TCPDebug_checkBox";
            this.TCPDebug_checkBox.Size = new System.Drawing.Size(82, 17);
            this.TCPDebug_checkBox.TabIndex = 18;
            this.TCPDebug_checkBox.Text = "TCP Debug";
            this.TCPDebug_checkBox.UseVisualStyleBackColor = true;
            // 
            // InvertX_checkBox
            // 
            this.InvertX_checkBox.AutoSize = true;
            this.InvertX_checkBox.Location = new System.Drawing.Point(47, 115);
            this.InvertX_checkBox.Name = "InvertX_checkBox";
            this.InvertX_checkBox.Size = new System.Drawing.Size(104, 17);
            this.InvertX_checkBox.TabIndex = 19;
            this.InvertX_checkBox.Text = "Invert X (Aileron)";
            this.InvertX_checkBox.UseVisualStyleBackColor = true;
            // 
            // InvertY_checkBox
            // 
            this.InvertY_checkBox.AutoSize = true;
            this.InvertY_checkBox.Checked = true;
            this.InvertY_checkBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.InvertY_checkBox.Location = new System.Drawing.Point(47, 138);
            this.InvertY_checkBox.Name = "InvertY_checkBox";
            this.InvertY_checkBox.Size = new System.Drawing.Size(111, 17);
            this.InvertY_checkBox.TabIndex = 20;
            this.InvertY_checkBox.Text = "Invert Y (Elevator)";
            this.InvertY_checkBox.UseVisualStyleBackColor = true;
            // 
            // InvertZ_checkBox
            // 
            this.InvertZ_checkBox.AutoSize = true;
            this.InvertZ_checkBox.Location = new System.Drawing.Point(47, 161);
            this.InvertZ_checkBox.Name = "InvertZ_checkBox";
            this.InvertZ_checkBox.Size = new System.Drawing.Size(108, 17);
            this.InvertZ_checkBox.TabIndex = 21;
            this.InvertZ_checkBox.Text = "Invert Z (Throttle)";
            this.InvertZ_checkBox.UseVisualStyleBackColor = true;
            // 
            // JoyIP_checkBox
            // 
            this.JoyIP_checkBox.AutoSize = true;
            this.JoyIP_checkBox.Location = new System.Drawing.Point(111, 171);
            this.JoyIP_checkBox.Name = "JoyIP_checkBox";
            this.JoyIP_checkBox.Size = new System.Drawing.Size(101, 17);
            this.JoyIP_checkBox.TabIndex = 22;
            this.JoyIP_checkBox.Text = "Joystick out IP?";
            this.JoyIP_checkBox.UseVisualStyleBackColor = true;
            // 
            // Aileron_trackBar
            // 
            this.Aileron_trackBar.Location = new System.Drawing.Point(59, 34);
            this.Aileron_trackBar.Maximum = 100;
            this.Aileron_trackBar.Minimum = -100;
            this.Aileron_trackBar.Name = "Aileron_trackBar";
            this.Aileron_trackBar.Size = new System.Drawing.Size(200, 45);
            this.Aileron_trackBar.SmallChange = 20;
            this.Aileron_trackBar.TabIndex = 23;
            this.Aileron_trackBar.TickFrequency = 20;
            // 
            // Elevator_trackBar
            // 
            this.Elevator_trackBar.Location = new System.Drawing.Point(59, 85);
            this.Elevator_trackBar.Maximum = 100;
            this.Elevator_trackBar.Minimum = -100;
            this.Elevator_trackBar.Name = "Elevator_trackBar";
            this.Elevator_trackBar.Size = new System.Drawing.Size(200, 45);
            this.Elevator_trackBar.SmallChange = 20;
            this.Elevator_trackBar.TabIndex = 24;
            this.Elevator_trackBar.TickFrequency = 20;
            // 
            // Rudder_trackBar
            // 
            this.Rudder_trackBar.Enabled = false;
            this.Rudder_trackBar.Location = new System.Drawing.Point(59, 136);
            this.Rudder_trackBar.Maximum = 100;
            this.Rudder_trackBar.Minimum = -100;
            this.Rudder_trackBar.Name = "Rudder_trackBar";
            this.Rudder_trackBar.Size = new System.Drawing.Size(200, 45);
            this.Rudder_trackBar.SmallChange = 20;
            this.Rudder_trackBar.TabIndex = 25;
            this.Rudder_trackBar.TickFrequency = 20;
            // 
            // OverrideJoy_checkBox
            // 
            this.OverrideJoy_checkBox.AutoSize = true;
            this.OverrideJoy_checkBox.Location = new System.Drawing.Point(154, 255);
            this.OverrideJoy_checkBox.Name = "OverrideJoy_checkBox";
            this.OverrideJoy_checkBox.Size = new System.Drawing.Size(131, 17);
            this.OverrideJoy_checkBox.TabIndex = 26;
            this.OverrideJoy_checkBox.Text = "Overide Joystick Input";
            this.OverrideJoy_checkBox.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(22, 45);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(39, 13);
            this.label3.TabIndex = 27;
            this.label3.Text = "Aileron";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(15, 96);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(46, 13);
            this.label4.TabIndex = 28;
            this.label4.Text = "Elevator";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Enabled = false;
            this.label5.Location = new System.Drawing.Point(19, 145);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(42, 13);
            this.label5.TabIndex = 29;
            this.label5.Text = "Rudder";
            // 
            // CopyXtoZ_checkBox
            // 
            this.CopyXtoZ_checkBox.AutoSize = true;
            this.CopyXtoZ_checkBox.Location = new System.Drawing.Point(47, 200);
            this.CopyXtoZ_checkBox.Name = "CopyXtoZ_checkBox";
            this.CopyXtoZ_checkBox.Size = new System.Drawing.Size(97, 17);
            this.CopyXtoZ_checkBox.TabIndex = 30;
            this.CopyXtoZ_checkBox.Text = "Swap X and  Z";
            this.CopyXtoZ_checkBox.UseVisualStyleBackColor = true;
            // 
            // DynDNS_textBox
            // 
            this.DynDNS_textBox.Location = new System.Drawing.Point(111, 258);
            this.DynDNS_textBox.Multiline = true;
            this.DynDNS_textBox.Name = "DynDNS_textBox";
            this.DynDNS_textBox.Size = new System.Drawing.Size(115, 24);
            this.DynDNS_textBox.TabIndex = 31;
            this.DynDNS_textBox.Text = "tomtest.no-ip.org";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(53, 261);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(52, 13);
            this.label6.TabIndex = 32;
            this.label6.Text = "DynDNS:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(40, 291);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 13);
            this.label7.TabIndex = 33;
            this.label7.Text = "Flight Mode:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(53, 231);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 35;
            this.label1.Text = "Port:";
            // 
            // Port_textBox
            // 
            this.Port_textBox.Location = new System.Drawing.Point(111, 228);
            this.Port_textBox.Multiline = true;
            this.Port_textBox.Name = "Port_textBox";
            this.Port_textBox.Size = new System.Drawing.Size(59, 24);
            this.Port_textBox.TabIndex = 34;
            this.Port_textBox.Text = "3003";
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.JoyCfg_tabPage);
            this.tabControl1.Controls.Add(this.JoyData_tabPage);
            this.tabControl1.Controls.Add(this.Connection_tabPage);
            this.tabControl1.Controls.Add(this.Debug_tabPage);
            this.tabControl1.Controls.Add(this.About_tabPage);
            this.tabControl1.Location = new System.Drawing.Point(5, 10);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(326, 397);
            this.tabControl1.TabIndex = 36;
            // 
            // JoyCfg_tabPage
            // 
            this.JoyCfg_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.JoyCfg_tabPage.Controls.Add(this.CopyYtoZ_checkBox);
            this.JoyCfg_tabPage.Controls.Add(this.CopyXtoY_checkBox);
            this.JoyCfg_tabPage.Controls.Add(this.joystickComboBox);
            this.JoyCfg_tabPage.Controls.Add(this.IsJoyFound_label);
            this.JoyCfg_tabPage.Controls.Add(this.InvertX_checkBox);
            this.JoyCfg_tabPage.Controls.Add(this.InvertY_checkBox);
            this.JoyCfg_tabPage.Controls.Add(this.InvertZ_checkBox);
            this.JoyCfg_tabPage.Controls.Add(this.CopyXtoZ_checkBox);
            this.JoyCfg_tabPage.Location = new System.Drawing.Point(4, 22);
            this.JoyCfg_tabPage.Name = "JoyCfg_tabPage";
            this.JoyCfg_tabPage.Padding = new System.Windows.Forms.Padding(3);
            this.JoyCfg_tabPage.Size = new System.Drawing.Size(318, 371);
            this.JoyCfg_tabPage.TabIndex = 1;
            this.JoyCfg_tabPage.Text = "Joystick Cfg";
            // 
            // CopyYtoZ_checkBox
            // 
            this.CopyYtoZ_checkBox.AutoSize = true;
            this.CopyYtoZ_checkBox.Location = new System.Drawing.Point(47, 246);
            this.CopyYtoZ_checkBox.Name = "CopyYtoZ_checkBox";
            this.CopyYtoZ_checkBox.Size = new System.Drawing.Size(97, 17);
            this.CopyYtoZ_checkBox.TabIndex = 32;
            this.CopyYtoZ_checkBox.Text = "Swap Y and  Z";
            this.CopyYtoZ_checkBox.UseVisualStyleBackColor = true;
            // 
            // CopyXtoY_checkBox
            // 
            this.CopyXtoY_checkBox.AutoSize = true;
            this.CopyXtoY_checkBox.Location = new System.Drawing.Point(47, 223);
            this.CopyXtoY_checkBox.Name = "CopyXtoY_checkBox";
            this.CopyXtoY_checkBox.Size = new System.Drawing.Size(97, 17);
            this.CopyXtoY_checkBox.TabIndex = 31;
            this.CopyXtoY_checkBox.Text = "Swap X and  Y";
            this.CopyXtoY_checkBox.UseVisualStyleBackColor = true;
            // 
            // JoyData_tabPage
            // 
            this.JoyData_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.JoyData_tabPage.Controls.Add(this.label8);
            this.JoyData_tabPage.Controls.Add(this.Throttle_trackBar);
            this.JoyData_tabPage.Controls.Add(this.JoyReset_button);
            this.JoyData_tabPage.Controls.Add(this.Aileron_trackBar);
            this.JoyData_tabPage.Controls.Add(this.Elevator_trackBar);
            this.JoyData_tabPage.Controls.Add(this.Rudder_trackBar);
            this.JoyData_tabPage.Controls.Add(this.OverrideJoy_checkBox);
            this.JoyData_tabPage.Controls.Add(this.label3);
            this.JoyData_tabPage.Controls.Add(this.label4);
            this.JoyData_tabPage.Controls.Add(this.label5);
            this.JoyData_tabPage.Location = new System.Drawing.Point(4, 22);
            this.JoyData_tabPage.Name = "JoyData_tabPage";
            this.JoyData_tabPage.Size = new System.Drawing.Size(318, 371);
            this.JoyData_tabPage.TabIndex = 2;
            this.JoyData_tabPage.Text = "Joystick Data";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(20, 247);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(43, 13);
            this.label8.TabIndex = 40;
            this.label8.Text = "Throttle";
            // 
            // Throttle_trackBar
            // 
            this.Throttle_trackBar.Location = new System.Drawing.Point(71, 187);
            this.Throttle_trackBar.Maximum = 100;
            this.Throttle_trackBar.Minimum = -100;
            this.Throttle_trackBar.Name = "Throttle_trackBar";
            this.Throttle_trackBar.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.Throttle_trackBar.Size = new System.Drawing.Size(45, 146);
            this.Throttle_trackBar.SmallChange = 20;
            this.Throttle_trackBar.TabIndex = 39;
            this.Throttle_trackBar.TickFrequency = 20;
            // 
            // JoyReset_button
            // 
            this.JoyReset_button.Location = new System.Drawing.Point(158, 278);
            this.JoyReset_button.Name = "JoyReset_button";
            this.JoyReset_button.Size = new System.Drawing.Size(101, 32);
            this.JoyReset_button.TabIndex = 38;
            this.JoyReset_button.Text = "Reset";
            this.JoyReset_button.UseVisualStyleBackColor = true;
            this.JoyReset_button.Click += new System.EventHandler(this.JoyReset_button_Click);
            // 
            // Connection_tabPage
            // 
            this.Connection_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.Connection_tabPage.Controls.Add(this.DataRx_label);
            this.Connection_tabPage.Controls.Add(this.groupBox2);
            this.Connection_tabPage.Controls.Add(this.groupBox1);
            this.Connection_tabPage.Controls.Add(this.JoyIP_checkBox);
            this.Connection_tabPage.Controls.Add(this.port_label);
            this.Connection_tabPage.Controls.Add(this.EnableTCPServer_checkBox);
            this.Connection_tabPage.Controls.Add(this.radioButton1);
            this.Connection_tabPage.Controls.Add(this.label1);
            this.Connection_tabPage.Controls.Add(this.Port_textBox);
            this.Connection_tabPage.Controls.Add(this.Mode_comboBox);
            this.Connection_tabPage.Controls.Add(this.label7);
            this.Connection_tabPage.Controls.Add(this.DynDNS_textBox);
            this.Connection_tabPage.Controls.Add(this.label6);
            this.Connection_tabPage.Location = new System.Drawing.Point(4, 22);
            this.Connection_tabPage.Name = "Connection_tabPage";
            this.Connection_tabPage.Size = new System.Drawing.Size(318, 371);
            this.Connection_tabPage.TabIndex = 3;
            this.Connection_tabPage.Text = "Connection";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.ModeServer_radioButton);
            this.groupBox2.Controls.Add(this.ModeClient_radioButton);
            this.groupBox2.Location = new System.Drawing.Point(146, 29);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(112, 69);
            this.groupBox2.TabIndex = 37;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Mode";
            // 
            // ModeServer_radioButton
            // 
            this.ModeServer_radioButton.AutoSize = true;
            this.ModeServer_radioButton.Checked = true;
            this.ModeServer_radioButton.Location = new System.Drawing.Point(4, 19);
            this.ModeServer_radioButton.Name = "ModeServer_radioButton";
            this.ModeServer_radioButton.Size = new System.Drawing.Size(102, 17);
            this.ModeServer_radioButton.TabIndex = 37;
            this.ModeServer_radioButton.TabStop = true;
            this.ModeServer_radioButton.Text = "Server (Listener)";
            this.ModeServer_radioButton.UseVisualStyleBackColor = true;
            // 
            // ModeClient_radioButton
            // 
            this.ModeClient_radioButton.AutoSize = true;
            this.ModeClient_radioButton.Location = new System.Drawing.Point(4, 42);
            this.ModeClient_radioButton.Name = "ModeClient_radioButton";
            this.ModeClient_radioButton.Size = new System.Drawing.Size(51, 17);
            this.ModeClient_radioButton.TabIndex = 0;
            this.ModeClient_radioButton.Text = "Client";
            this.ModeClient_radioButton.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.IpTypeUDP_radioButton);
            this.groupBox1.Controls.Add(this.IpTypeTCP_radioButton);
            this.groupBox1.Location = new System.Drawing.Point(50, 29);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(72, 69);
            this.groupBox1.TabIndex = 36;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "IP Type";
            // 
            // IpTypeUDP_radioButton
            // 
            this.IpTypeUDP_radioButton.AutoSize = true;
            this.IpTypeUDP_radioButton.Location = new System.Drawing.Point(6, 42);
            this.IpTypeUDP_radioButton.Name = "IpTypeUDP_radioButton";
            this.IpTypeUDP_radioButton.Size = new System.Drawing.Size(48, 17);
            this.IpTypeUDP_radioButton.TabIndex = 37;
            this.IpTypeUDP_radioButton.Text = "UDP";
            this.IpTypeUDP_radioButton.UseVisualStyleBackColor = true;
            // 
            // IpTypeTCP_radioButton
            // 
            this.IpTypeTCP_radioButton.AutoSize = true;
            this.IpTypeTCP_radioButton.Checked = true;
            this.IpTypeTCP_radioButton.Location = new System.Drawing.Point(6, 19);
            this.IpTypeTCP_radioButton.Name = "IpTypeTCP_radioButton";
            this.IpTypeTCP_radioButton.Size = new System.Drawing.Size(46, 17);
            this.IpTypeTCP_radioButton.TabIndex = 0;
            this.IpTypeTCP_radioButton.TabStop = true;
            this.IpTypeTCP_radioButton.Text = "TCP";
            this.IpTypeTCP_radioButton.UseVisualStyleBackColor = true;
            // 
            // Debug_tabPage
            // 
            this.Debug_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.Debug_tabPage.Controls.Add(this.JoyStickDebug_checkBox);
            this.Debug_tabPage.Controls.Add(this.TextBoxClear_button);
            this.Debug_tabPage.Controls.Add(this.textBox1);
            this.Debug_tabPage.Controls.Add(this.TCPDebug_checkBox);
            this.Debug_tabPage.Location = new System.Drawing.Point(4, 22);
            this.Debug_tabPage.Name = "Debug_tabPage";
            this.Debug_tabPage.Padding = new System.Windows.Forms.Padding(3);
            this.Debug_tabPage.Size = new System.Drawing.Size(318, 371);
            this.Debug_tabPage.TabIndex = 0;
            this.Debug_tabPage.Text = "Debug";
            // 
            // JoyStickDebug_checkBox
            // 
            this.JoyStickDebug_checkBox.AutoSize = true;
            this.JoyStickDebug_checkBox.Location = new System.Drawing.Point(6, 351);
            this.JoyStickDebug_checkBox.Name = "JoyStickDebug_checkBox";
            this.JoyStickDebug_checkBox.Size = new System.Drawing.Size(99, 17);
            this.JoyStickDebug_checkBox.TabIndex = 39;
            this.JoyStickDebug_checkBox.Text = "Joystick Debug";
            this.JoyStickDebug_checkBox.UseVisualStyleBackColor = true;
            // 
            // TextBoxClear_button
            // 
            this.TextBoxClear_button.Location = new System.Drawing.Point(199, 332);
            this.TextBoxClear_button.Name = "TextBoxClear_button";
            this.TextBoxClear_button.Size = new System.Drawing.Size(87, 32);
            this.TextBoxClear_button.TabIndex = 38;
            this.TextBoxClear_button.Text = "Clear";
            this.TextBoxClear_button.UseVisualStyleBackColor = true;
            this.TextBoxClear_button.Click += new System.EventHandler(this.TextBoxClear_button_Click);
            // 
            // About_tabPage
            // 
            this.About_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.About_tabPage.Controls.Add(this.label2);
            this.About_tabPage.Controls.Add(this.EmailTomPittenger_button);
            this.About_tabPage.Location = new System.Drawing.Point(4, 22);
            this.About_tabPage.Name = "About_tabPage";
            this.About_tabPage.Size = new System.Drawing.Size(318, 371);
            this.About_tabPage.TabIndex = 4;
            this.About_tabPage.Text = "About";
            // 
            // HouseKeeping_1sec_timer
            // 
            this.HouseKeeping_1sec_timer.Enabled = true;
            this.HouseKeeping_1sec_timer.Interval = 1000;
            this.HouseKeeping_1sec_timer.Tick += new System.EventHandler(this.HouseKeeping_1sec_timer_Tick);
            // 
            // DataRx_label
            // 
            this.DataRx_label.AutoSize = true;
            this.DataRx_label.Location = new System.Drawing.Point(53, 111);
            this.DataRx_label.Name = "DataRx_label";
            this.DataRx_label.Size = new System.Drawing.Size(49, 13);
            this.DataRx_label.TabIndex = 38;
            this.DataRx_label.Text = "Data Rx:";
            // 
            // EmailTomPittenger_button
            // 
            this.EmailTomPittenger_button.Location = new System.Drawing.Point(54, 96);
            this.EmailTomPittenger_button.Name = "EmailTomPittenger_button";
            this.EmailTomPittenger_button.Size = new System.Drawing.Size(75, 23);
            this.EmailTomPittenger_button.TabIndex = 1;
            this.EmailTomPittenger_button.Text = "Email Tom!";
            this.EmailTomPittenger_button.UseVisualStyleBackColor = true;
            this.EmailTomPittenger_button.Click += new System.EventHandler(this.EmailTomPittenger_button_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(51, 68);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(179, 13);
            this.label2.TabIndex = 28;
            this.label2.Text = "UDB \"Fly-By-Wire\" by Tom Pittenger";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnablePreventFocusChange;
            this.ClientSize = new System.Drawing.Size(334, 408);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "UAV Dev Board \"Fly-By-Wire\"";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.Aileron_trackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Elevator_trackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Rudder_trackBar)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.JoyCfg_tabPage.ResumeLayout(false);
            this.JoyCfg_tabPage.PerformLayout();
            this.JoyData_tabPage.ResumeLayout(false);
            this.JoyData_tabPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Throttle_trackBar)).EndInit();
            this.Connection_tabPage.ResumeLayout(false);
            this.Connection_tabPage.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.Debug_tabPage.ResumeLayout(false);
            this.Debug_tabPage.PerformLayout();
            this.About_tabPage.ResumeLayout(false);
            this.About_tabPage.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Timer timer_data;
        private System.Windows.Forms.Timer timer_status;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.ComboBox joystickComboBox;
        private System.Windows.Forms.Timer Joystick_timer;
        private System.Windows.Forms.Label IsJoyFound_label;
        private System.Windows.Forms.Label port_label;
        private System.Windows.Forms.CheckBox EnableTCPServer_checkBox;
        private System.Windows.Forms.ComboBox Mode_comboBox;
        private System.Windows.Forms.CheckBox TCPDebug_checkBox;
        private System.Windows.Forms.CheckBox InvertX_checkBox;
        private System.Windows.Forms.CheckBox InvertY_checkBox;
        private System.Windows.Forms.CheckBox InvertZ_checkBox;
        private System.Windows.Forms.CheckBox JoyIP_checkBox;
        private System.Windows.Forms.TrackBar Aileron_trackBar;
        private System.Windows.Forms.TrackBar Elevator_trackBar;
        private System.Windows.Forms.TrackBar Rudder_trackBar;
        private System.Windows.Forms.CheckBox OverrideJoy_checkBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox CopyXtoZ_checkBox;
        private System.Windows.Forms.TextBox DynDNS_textBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox Port_textBox;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage Debug_tabPage;
        private System.Windows.Forms.TabPage JoyCfg_tabPage;
        private System.Windows.Forms.TabPage JoyData_tabPage;
        private System.Windows.Forms.TabPage Connection_tabPage;
        private System.Windows.Forms.Button JoyReset_button;
        private System.Windows.Forms.Button TextBoxClear_button;
        private System.Windows.Forms.RadioButton IpTypeUDP_radioButton;
        private System.Windows.Forms.RadioButton IpTypeTCP_radioButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton ModeServer_radioButton;
        private System.Windows.Forms.RadioButton ModeClient_radioButton;
        private System.Windows.Forms.TabPage About_tabPage;
        private System.Windows.Forms.CheckBox JoyStickDebug_checkBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TrackBar Throttle_trackBar;
        private System.Windows.Forms.CheckBox CopyYtoZ_checkBox;
        private System.Windows.Forms.CheckBox CopyXtoY_checkBox;
        private System.Windows.Forms.Timer HouseKeeping_1sec_timer;
        private System.Windows.Forms.Label DataRx_label;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button EmailTomPittenger_button;
    }
}

