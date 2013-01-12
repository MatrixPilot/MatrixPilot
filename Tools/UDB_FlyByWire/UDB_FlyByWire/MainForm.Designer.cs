namespace UDB_FlyByWire
{
    partial class MainForm
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
            this.debug_textBox = new System.Windows.Forms.TextBox();
            this.IsConnected_radioButton = new System.Windows.Forms.RadioButton();
            this.Joystick_timer = new System.Windows.Forms.Timer(this.components);
            this.IpDebug_checkBox = new System.Windows.Forms.CheckBox();
            this.Aileron_trackBar = new System.Windows.Forms.TrackBar();
            this.Elevator_trackBar = new System.Windows.Forms.TrackBar();
            this.Rudder_trackBar = new System.Windows.Forms.TrackBar();
            this.OverrideJoy_checkBox = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.Port_textBox = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.Joy_tabPage = new System.Windows.Forms.TabPage();
            this.InvertThrottle_checkBox = new System.Windows.Forms.CheckBox();
            this.InvertRudder_checkBox = new System.Windows.Forms.CheckBox();
            this.label19 = new System.Windows.Forms.Label();
            this.InvertAileron_checkBox = new System.Windows.Forms.CheckBox();
            this.label18 = new System.Windows.Forms.Label();
            this.ThrottleTrim_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.RudderTrim_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.ElevatorTrim_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label17 = new System.Windows.Forms.Label();
            this.AileronTrim_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.JoystickSetCenter_button = new System.Windows.Forms.Button();
            this.Mode_comboBox = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.InvertElevator_checkBox = new System.Windows.Forms.CheckBox();
            this.joystickComboBox = new System.Windows.Forms.ComboBox();
            this.IsJoyFound_label = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.Throttle_trackBar = new System.Windows.Forms.TrackBar();
            this.JoyReset_button = new System.Windows.Forms.Button();
            this.Connection_tabPage = new System.Windows.Forms.TabPage();
            this.IP_groupBox = new System.Windows.Forms.GroupBox();
            this.ClientIP_textBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.Serial_groupBox = new System.Windows.Forms.GroupBox();
            this.CommSerialPort_comboBox = new System.Windows.Forms.ComboBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.CommSerialBaud_comboBox = new System.Windows.Forms.ComboBox();
            this.UploadRate_label = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.UpldateRate_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.Connect_checkBox = new System.Windows.Forms.CheckBox();
            this.ClientDisconnect_button = new System.Windows.Forms.Button();
            this.IPmode_groupBox = new System.Windows.Forms.GroupBox();
            this.IpModeServer_radioButton = new System.Windows.Forms.RadioButton();
            this.IpModeClient_radioButton = new System.Windows.Forms.RadioButton();
            this.IPtype_groupBox = new System.Windows.Forms.GroupBox();
            this.CommTypeSerial_radioButton = new System.Windows.Forms.RadioButton();
            this.CommTypeUDP_radioButton = new System.Windows.Forms.RadioButton();
            this.CommTypeTCP_radioButton = new System.Windows.Forms.RadioButton();
            this.Debug_tabPage = new System.Windows.Forms.TabPage();
            this.JoyStickDebug_checkBox = new System.Windows.Forms.CheckBox();
            this.TextBoxClear_button = new System.Windows.Forms.Button();
            this.About_tabPage = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.EmailTomPittenger_button = new System.Windows.Forms.Button();
            this.HouseKeeping_1sec_timer = new System.Windows.Forms.Timer(this.components);
            this.Housekeeping_timer = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.Aileron_trackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Elevator_trackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Rudder_trackBar)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.Joy_tabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ThrottleTrim_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RudderTrim_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ElevatorTrim_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AileronTrim_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Throttle_trackBar)).BeginInit();
            this.Connection_tabPage.SuspendLayout();
            this.IP_groupBox.SuspendLayout();
            this.Serial_groupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.UpldateRate_numericUpDown)).BeginInit();
            this.IPmode_groupBox.SuspendLayout();
            this.IPtype_groupBox.SuspendLayout();
            this.Debug_tabPage.SuspendLayout();
            this.About_tabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // debug_textBox
            // 
            this.debug_textBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.debug_textBox.Location = new System.Drawing.Point(3, 6);
            this.debug_textBox.Multiline = true;
            this.debug_textBox.Name = "debug_textBox";
            this.debug_textBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.debug_textBox.Size = new System.Drawing.Size(340, 320);
            this.debug_textBox.TabIndex = 0;
            // 
            // IsConnected_radioButton
            // 
            this.IsConnected_radioButton.AutoSize = true;
            this.IsConnected_radioButton.Checked = true;
            this.IsConnected_radioButton.Location = new System.Drawing.Point(232, 114);
            this.IsConnected_radioButton.Name = "IsConnected_radioButton";
            this.IsConnected_radioButton.Size = new System.Drawing.Size(77, 17);
            this.IsConnected_radioButton.TabIndex = 2;
            this.IsConnected_radioButton.TabStop = true;
            this.IsConnected_radioButton.Text = "Connected";
            this.IsConnected_radioButton.UseVisualStyleBackColor = true;
            this.IsConnected_radioButton.MouseClick += new System.Windows.Forms.MouseEventHandler(this.IsConnected_radioButton_MouseClick);
            // 
            // Joystick_timer
            // 
            this.Joystick_timer.Enabled = true;
            this.Joystick_timer.Interval = 25;
            this.Joystick_timer.Tick += new System.EventHandler(this.Joystick_timer_Tick);
            // 
            // IpDebug_checkBox
            // 
            this.IpDebug_checkBox.AutoSize = true;
            this.IpDebug_checkBox.Location = new System.Drawing.Point(6, 332);
            this.IpDebug_checkBox.Name = "IpDebug_checkBox";
            this.IpDebug_checkBox.Size = new System.Drawing.Size(71, 17);
            this.IpDebug_checkBox.TabIndex = 18;
            this.IpDebug_checkBox.Text = "IP Debug";
            this.IpDebug_checkBox.UseVisualStyleBackColor = true;
            this.IpDebug_checkBox.CheckedChanged += new System.EventHandler(this.IpDebug_checkBox_CheckedChanged);
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
            this.Aileron_trackBar.TickFrequency = 10;
            this.Aileron_trackBar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Aileron_trackBar_MouseUp);
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
            this.Elevator_trackBar.TickFrequency = 10;
            this.Elevator_trackBar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Elevator_trackBar_MouseUp);
            // 
            // Rudder_trackBar
            // 
            this.Rudder_trackBar.Location = new System.Drawing.Point(59, 136);
            this.Rudder_trackBar.Maximum = 100;
            this.Rudder_trackBar.Minimum = -100;
            this.Rudder_trackBar.Name = "Rudder_trackBar";
            this.Rudder_trackBar.Size = new System.Drawing.Size(200, 45);
            this.Rudder_trackBar.SmallChange = 20;
            this.Rudder_trackBar.TabIndex = 25;
            this.Rudder_trackBar.TickFrequency = 10;
            this.Rudder_trackBar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Rudder_trackBar_MouseUp);
            // 
            // OverrideJoy_checkBox
            // 
            this.OverrideJoy_checkBox.AutoSize = true;
            this.OverrideJoy_checkBox.Location = new System.Drawing.Point(193, 259);
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
            this.label5.Location = new System.Drawing.Point(19, 145);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(42, 13);
            this.label5.TabIndex = 29;
            this.label5.Text = "Rudder";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 52);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 35;
            this.label1.Text = "Port:";
            // 
            // Port_textBox
            // 
            this.Port_textBox.Location = new System.Drawing.Point(57, 49);
            this.Port_textBox.Multiline = true;
            this.Port_textBox.Name = "Port_textBox";
            this.Port_textBox.Size = new System.Drawing.Size(59, 24);
            this.Port_textBox.TabIndex = 34;
            this.Port_textBox.Text = "3003";
            this.Port_textBox.TextChanged += new System.EventHandler(this.Port_textBox_TextChanged);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.Joy_tabPage);
            this.tabControl1.Controls.Add(this.Connection_tabPage);
            this.tabControl1.Controls.Add(this.Debug_tabPage);
            this.tabControl1.Controls.Add(this.About_tabPage);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(357, 399);
            this.tabControl1.TabIndex = 36;
            // 
            // Joy_tabPage
            // 
            this.Joy_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.Joy_tabPage.Controls.Add(this.InvertThrottle_checkBox);
            this.Joy_tabPage.Controls.Add(this.InvertRudder_checkBox);
            this.Joy_tabPage.Controls.Add(this.label19);
            this.Joy_tabPage.Controls.Add(this.InvertAileron_checkBox);
            this.Joy_tabPage.Controls.Add(this.label18);
            this.Joy_tabPage.Controls.Add(this.ThrottleTrim_numericUpDown);
            this.Joy_tabPage.Controls.Add(this.RudderTrim_numericUpDown);
            this.Joy_tabPage.Controls.Add(this.ElevatorTrim_numericUpDown);
            this.Joy_tabPage.Controls.Add(this.label17);
            this.Joy_tabPage.Controls.Add(this.AileronTrim_numericUpDown);
            this.Joy_tabPage.Controls.Add(this.label13);
            this.Joy_tabPage.Controls.Add(this.label12);
            this.Joy_tabPage.Controls.Add(this.label11);
            this.Joy_tabPage.Controls.Add(this.label10);
            this.Joy_tabPage.Controls.Add(this.label6);
            this.Joy_tabPage.Controls.Add(this.JoystickSetCenter_button);
            this.Joy_tabPage.Controls.Add(this.Mode_comboBox);
            this.Joy_tabPage.Controls.Add(this.label7);
            this.Joy_tabPage.Controls.Add(this.InvertElevator_checkBox);
            this.Joy_tabPage.Controls.Add(this.joystickComboBox);
            this.Joy_tabPage.Controls.Add(this.IsJoyFound_label);
            this.Joy_tabPage.Controls.Add(this.label8);
            this.Joy_tabPage.Controls.Add(this.Throttle_trackBar);
            this.Joy_tabPage.Controls.Add(this.JoyReset_button);
            this.Joy_tabPage.Controls.Add(this.Aileron_trackBar);
            this.Joy_tabPage.Controls.Add(this.Elevator_trackBar);
            this.Joy_tabPage.Controls.Add(this.Rudder_trackBar);
            this.Joy_tabPage.Controls.Add(this.OverrideJoy_checkBox);
            this.Joy_tabPage.Controls.Add(this.label3);
            this.Joy_tabPage.Controls.Add(this.label4);
            this.Joy_tabPage.Controls.Add(this.label5);
            this.Joy_tabPage.Location = new System.Drawing.Point(4, 22);
            this.Joy_tabPage.Name = "Joy_tabPage";
            this.Joy_tabPage.Size = new System.Drawing.Size(349, 373);
            this.Joy_tabPage.TabIndex = 2;
            this.Joy_tabPage.Text = "Joystick";
            // 
            // InvertThrottle_checkBox
            // 
            this.InvertThrottle_checkBox.AutoSize = true;
            this.InvertThrottle_checkBox.Location = new System.Drawing.Point(91, 320);
            this.InvertThrottle_checkBox.Name = "InvertThrottle_checkBox";
            this.InvertThrottle_checkBox.Size = new System.Drawing.Size(15, 14);
            this.InvertThrottle_checkBox.TabIndex = 61;
            this.InvertThrottle_checkBox.UseVisualStyleBackColor = true;
            this.InvertThrottle_checkBox.CheckedChanged += new System.EventHandler(this.InvertThrottle_checkBox_CheckedChanged);
            // 
            // InvertRudder_checkBox
            // 
            this.InvertRudder_checkBox.AutoSize = true;
            this.InvertRudder_checkBox.Location = new System.Drawing.Point(319, 138);
            this.InvertRudder_checkBox.Name = "InvertRudder_checkBox";
            this.InvertRudder_checkBox.Size = new System.Drawing.Size(15, 14);
            this.InvertRudder_checkBox.TabIndex = 60;
            this.InvertRudder_checkBox.UseVisualStyleBackColor = true;
            this.InvertRudder_checkBox.CheckedChanged += new System.EventHandler(this.InvertRudder_checkBox_CheckedChanged);
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(312, 18);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(34, 13);
            this.label19.TabIndex = 59;
            this.label19.Text = "Invert";
            // 
            // InvertAileron_checkBox
            // 
            this.InvertAileron_checkBox.AutoSize = true;
            this.InvertAileron_checkBox.Location = new System.Drawing.Point(319, 36);
            this.InvertAileron_checkBox.Name = "InvertAileron_checkBox";
            this.InvertAileron_checkBox.Size = new System.Drawing.Size(15, 14);
            this.InvertAileron_checkBox.TabIndex = 58;
            this.InvertAileron_checkBox.UseVisualStyleBackColor = true;
            this.InvertAileron_checkBox.CheckedChanged += new System.EventHandler(this.InvertAileron_checkBox_CheckedChanged);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(10, 320);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(27, 13);
            this.label18.TabIndex = 57;
            this.label18.Text = "Trim";
            // 
            // ThrottleTrim_numericUpDown
            // 
            this.ThrottleTrim_numericUpDown.Location = new System.Drawing.Point(41, 318);
            this.ThrottleTrim_numericUpDown.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.ThrottleTrim_numericUpDown.Name = "ThrottleTrim_numericUpDown";
            this.ThrottleTrim_numericUpDown.Size = new System.Drawing.Size(44, 20);
            this.ThrottleTrim_numericUpDown.TabIndex = 56;
            // 
            // RudderTrim_numericUpDown
            // 
            this.RudderTrim_numericUpDown.Location = new System.Drawing.Point(265, 136);
            this.RudderTrim_numericUpDown.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.RudderTrim_numericUpDown.Name = "RudderTrim_numericUpDown";
            this.RudderTrim_numericUpDown.Size = new System.Drawing.Size(44, 20);
            this.RudderTrim_numericUpDown.TabIndex = 55;
            // 
            // ElevatorTrim_numericUpDown
            // 
            this.ElevatorTrim_numericUpDown.Location = new System.Drawing.Point(265, 85);
            this.ElevatorTrim_numericUpDown.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.ElevatorTrim_numericUpDown.Name = "ElevatorTrim_numericUpDown";
            this.ElevatorTrim_numericUpDown.Size = new System.Drawing.Size(44, 20);
            this.ElevatorTrim_numericUpDown.TabIndex = 54;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(275, 18);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(27, 13);
            this.label17.TabIndex = 53;
            this.label17.Text = "Trim";
            // 
            // AileronTrim_numericUpDown
            // 
            this.AileronTrim_numericUpDown.Location = new System.Drawing.Point(265, 34);
            this.AileronTrim_numericUpDown.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.AileronTrim_numericUpDown.Name = "AileronTrim_numericUpDown";
            this.AileronTrim_numericUpDown.Size = new System.Drawing.Size(44, 20);
            this.AileronTrim_numericUpDown.TabIndex = 52;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(153, 18);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(13, 13);
            this.label13.TabIndex = 51;
            this.label13.Text = "0";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(56, 18);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(36, 13);
            this.label12.TabIndex = 50;
            this.label12.Text = "-100%";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(223, 18);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(33, 13);
            this.label11.TabIndex = 49;
            this.label11.Text = "100%";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(28, 190);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(33, 13);
            this.label10.TabIndex = 48;
            this.label10.Text = "100%";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(38, 302);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(21, 13);
            this.label6.TabIndex = 47;
            this.label6.Text = "0%";
            // 
            // JoystickSetCenter_button
            // 
            this.JoystickSetCenter_button.Location = new System.Drawing.Point(182, 282);
            this.JoystickSetCenter_button.Name = "JoystickSetCenter_button";
            this.JoystickSetCenter_button.Size = new System.Drawing.Size(74, 32);
            this.JoystickSetCenter_button.TabIndex = 46;
            this.JoystickSetCenter_button.Text = "Calibrate";
            this.JoystickSetCenter_button.UseVisualStyleBackColor = true;
            this.JoystickSetCenter_button.Click += new System.EventHandler(this.JoystickSetCenter_button_Click);
            // 
            // Mode_comboBox
            // 
            this.Mode_comboBox.FormattingEnabled = true;
            this.Mode_comboBox.Items.AddRange(new object[] {
            "Manual",
            "Stabilized",
            "AutoPilot"});
            this.Mode_comboBox.Location = new System.Drawing.Point(175, 190);
            this.Mode_comboBox.Name = "Mode_comboBox";
            this.Mode_comboBox.Size = new System.Drawing.Size(72, 21);
            this.Mode_comboBox.TabIndex = 44;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(104, 190);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 13);
            this.label7.TabIndex = 45;
            this.label7.Text = "Flight Mode:";
            // 
            // InvertElevator_checkBox
            // 
            this.InvertElevator_checkBox.AutoSize = true;
            this.InvertElevator_checkBox.Location = new System.Drawing.Point(319, 87);
            this.InvertElevator_checkBox.Name = "InvertElevator_checkBox";
            this.InvertElevator_checkBox.Size = new System.Drawing.Size(15, 14);
            this.InvertElevator_checkBox.TabIndex = 43;
            this.InvertElevator_checkBox.UseVisualStyleBackColor = true;
            this.InvertElevator_checkBox.CheckedChanged += new System.EventHandler(this.InvertElevator_checkBox_CheckedChanged);
            // 
            // joystickComboBox
            // 
            this.joystickComboBox.FormattingEnabled = true;
            this.joystickComboBox.Location = new System.Drawing.Point(175, 333);
            this.joystickComboBox.Name = "joystickComboBox";
            this.joystickComboBox.Size = new System.Drawing.Size(149, 21);
            this.joystickComboBox.TabIndex = 41;
            // 
            // IsJoyFound_label
            // 
            this.IsJoyFound_label.AutoSize = true;
            this.IsJoyFound_label.Location = new System.Drawing.Point(229, 317);
            this.IsJoyFound_label.Name = "IsJoyFound_label";
            this.IsJoyFound_label.Size = new System.Drawing.Size(95, 13);
            this.IsJoyFound_label.TabIndex = 42;
            this.IsJoyFound_label.Text = "Is Joystick Found?";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(10, 245);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(43, 13);
            this.label8.TabIndex = 40;
            this.label8.Text = "Throttle";
            // 
            // Throttle_trackBar
            // 
            this.Throttle_trackBar.Location = new System.Drawing.Point(59, 177);
            this.Throttle_trackBar.Maximum = 100;
            this.Throttle_trackBar.Name = "Throttle_trackBar";
            this.Throttle_trackBar.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.Throttle_trackBar.Size = new System.Drawing.Size(45, 146);
            this.Throttle_trackBar.SmallChange = 20;
            this.Throttle_trackBar.TabIndex = 39;
            this.Throttle_trackBar.TickFrequency = 10;
            this.Throttle_trackBar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Throttle_trackBar_MouseUp);
            // 
            // JoyReset_button
            // 
            this.JoyReset_button.Location = new System.Drawing.Point(262, 282);
            this.JoyReset_button.Name = "JoyReset_button";
            this.JoyReset_button.Size = new System.Drawing.Size(69, 32);
            this.JoyReset_button.TabIndex = 38;
            this.JoyReset_button.Text = "Zero";
            this.JoyReset_button.UseVisualStyleBackColor = true;
            this.JoyReset_button.Click += new System.EventHandler(this.JoyReset_button_Click);
            // 
            // Connection_tabPage
            // 
            this.Connection_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.Connection_tabPage.Controls.Add(this.IP_groupBox);
            this.Connection_tabPage.Controls.Add(this.Serial_groupBox);
            this.Connection_tabPage.Controls.Add(this.UploadRate_label);
            this.Connection_tabPage.Controls.Add(this.label14);
            this.Connection_tabPage.Controls.Add(this.UpldateRate_numericUpDown);
            this.Connection_tabPage.Controls.Add(this.Connect_checkBox);
            this.Connection_tabPage.Controls.Add(this.ClientDisconnect_button);
            this.Connection_tabPage.Controls.Add(this.IPmode_groupBox);
            this.Connection_tabPage.Controls.Add(this.IPtype_groupBox);
            this.Connection_tabPage.Controls.Add(this.IsConnected_radioButton);
            this.Connection_tabPage.Location = new System.Drawing.Point(4, 22);
            this.Connection_tabPage.Name = "Connection_tabPage";
            this.Connection_tabPage.Size = new System.Drawing.Size(349, 373);
            this.Connection_tabPage.TabIndex = 3;
            this.Connection_tabPage.Text = "Connection";
            // 
            // IP_groupBox
            // 
            this.IP_groupBox.Controls.Add(this.ClientIP_textBox);
            this.IP_groupBox.Controls.Add(this.Port_textBox);
            this.IP_groupBox.Controls.Add(this.label1);
            this.IP_groupBox.Controls.Add(this.label9);
            this.IP_groupBox.Location = new System.Drawing.Point(14, 286);
            this.IP_groupBox.Name = "IP_groupBox";
            this.IP_groupBox.Size = new System.Drawing.Size(281, 79);
            this.IP_groupBox.TabIndex = 52;
            this.IP_groupBox.TabStop = false;
            this.IP_groupBox.Text = "IP stuff";
            // 
            // ClientIP_textBox
            // 
            this.ClientIP_textBox.Location = new System.Drawing.Point(57, 19);
            this.ClientIP_textBox.Multiline = true;
            this.ClientIP_textBox.Name = "ClientIP_textBox";
            this.ClientIP_textBox.Size = new System.Drawing.Size(202, 24);
            this.ClientIP_textBox.TabIndex = 41;
            this.ClientIP_textBox.Text = "192.168.11.200";
            this.ClientIP_textBox.TextChanged += new System.EventHandler(this.ClientIP_textBox_TextChanged);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(9, 22);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(49, 13);
            this.label9.TabIndex = 42;
            this.label9.Text = "Client IP:";
            // 
            // Serial_groupBox
            // 
            this.Serial_groupBox.Controls.Add(this.CommSerialPort_comboBox);
            this.Serial_groupBox.Controls.Add(this.label15);
            this.Serial_groupBox.Controls.Add(this.label16);
            this.Serial_groupBox.Controls.Add(this.CommSerialBaud_comboBox);
            this.Serial_groupBox.Location = new System.Drawing.Point(14, 193);
            this.Serial_groupBox.Name = "Serial_groupBox";
            this.Serial_groupBox.Size = new System.Drawing.Size(146, 87);
            this.Serial_groupBox.TabIndex = 51;
            this.Serial_groupBox.TabStop = false;
            this.Serial_groupBox.Text = "Serial";
            // 
            // CommSerialPort_comboBox
            // 
            this.CommSerialPort_comboBox.FormattingEnabled = true;
            this.CommSerialPort_comboBox.Items.AddRange(new object[] {
            "1200",
            "2400",
            "4800",
            "9600",
            "19200",
            "38400",
            "57600",
            "115200"});
            this.CommSerialPort_comboBox.Location = new System.Drawing.Point(63, 19);
            this.CommSerialPort_comboBox.Name = "CommSerialPort_comboBox";
            this.CommSerialPort_comboBox.Size = new System.Drawing.Size(65, 21);
            this.CommSerialPort_comboBox.TabIndex = 49;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(9, 49);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(35, 13);
            this.label15.TabIndex = 48;
            this.label15.Text = "Baud:";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(5, 22);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(56, 13);
            this.label16.TabIndex = 50;
            this.label16.Text = "COM Port:";
            // 
            // CommSerialBaud_comboBox
            // 
            this.CommSerialBaud_comboBox.FormattingEnabled = true;
            this.CommSerialBaud_comboBox.Items.AddRange(new object[] {
            "1200",
            "2400",
            "4800",
            "9600",
            "19200",
            "38400",
            "57600",
            "115200"});
            this.CommSerialBaud_comboBox.Location = new System.Drawing.Point(63, 46);
            this.CommSerialBaud_comboBox.Name = "CommSerialBaud_comboBox";
            this.CommSerialBaud_comboBox.Size = new System.Drawing.Size(65, 21);
            this.CommSerialBaud_comboBox.TabIndex = 47;
            // 
            // UploadRate_label
            // 
            this.UploadRate_label.AutoSize = true;
            this.UploadRate_label.Location = new System.Drawing.Point(148, 158);
            this.UploadRate_label.Name = "UploadRate_label";
            this.UploadRate_label.Size = new System.Drawing.Size(20, 13);
            this.UploadRate_label.TabIndex = 46;
            this.UploadRate_label.Text = "ms";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(17, 158);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(70, 13);
            this.label14.TabIndex = 45;
            this.label14.Text = "Upload Rate:";
            // 
            // UpldateRate_numericUpDown
            // 
            this.UpldateRate_numericUpDown.Location = new System.Drawing.Point(89, 156);
            this.UpldateRate_numericUpDown.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.UpldateRate_numericUpDown.Minimum = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.UpldateRate_numericUpDown.Name = "UpldateRate_numericUpDown";
            this.UpldateRate_numericUpDown.Size = new System.Drawing.Size(53, 20);
            this.UpldateRate_numericUpDown.TabIndex = 44;
            this.UpldateRate_numericUpDown.Value = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.UpldateRate_numericUpDown.ValueChanged += new System.EventHandler(this.UpldateRate_numericUpDown_ValueChanged);
            // 
            // Connect_checkBox
            // 
            this.Connect_checkBox.AutoSize = true;
            this.Connect_checkBox.Location = new System.Drawing.Point(241, 38);
            this.Connect_checkBox.Name = "Connect_checkBox";
            this.Connect_checkBox.Size = new System.Drawing.Size(54, 17);
            this.Connect_checkBox.TabIndex = 43;
            this.Connect_checkBox.Text = "Listen";
            this.Connect_checkBox.UseVisualStyleBackColor = true;
            this.Connect_checkBox.CheckedChanged += new System.EventHandler(this.Connect_checkBox_CheckedChanged);
            // 
            // ClientDisconnect_button
            // 
            this.ClientDisconnect_button.Location = new System.Drawing.Point(232, 61);
            this.ClientDisconnect_button.Name = "ClientDisconnect_button";
            this.ClientDisconnect_button.Size = new System.Drawing.Size(75, 27);
            this.ClientDisconnect_button.TabIndex = 40;
            this.ClientDisconnect_button.Text = "Disconnect";
            this.ClientDisconnect_button.UseVisualStyleBackColor = true;
            this.ClientDisconnect_button.Click += new System.EventHandler(this.ClientDisconnect_button_Click);
            // 
            // IPmode_groupBox
            // 
            this.IPmode_groupBox.Controls.Add(this.IpModeServer_radioButton);
            this.IPmode_groupBox.Controls.Add(this.IpModeClient_radioButton);
            this.IPmode_groupBox.Location = new System.Drawing.Point(114, 29);
            this.IPmode_groupBox.Name = "IPmode_groupBox";
            this.IPmode_groupBox.Size = new System.Drawing.Size(112, 69);
            this.IPmode_groupBox.TabIndex = 37;
            this.IPmode_groupBox.TabStop = false;
            this.IPmode_groupBox.Text = "Mode";
            // 
            // IpModeServer_radioButton
            // 
            this.IpModeServer_radioButton.AutoSize = true;
            this.IpModeServer_radioButton.Checked = true;
            this.IpModeServer_radioButton.Location = new System.Drawing.Point(4, 19);
            this.IpModeServer_radioButton.Name = "IpModeServer_radioButton";
            this.IpModeServer_radioButton.Size = new System.Drawing.Size(102, 17);
            this.IpModeServer_radioButton.TabIndex = 37;
            this.IpModeServer_radioButton.TabStop = true;
            this.IpModeServer_radioButton.Text = "Server (Listener)";
            this.IpModeServer_radioButton.UseVisualStyleBackColor = true;
            this.IpModeServer_radioButton.CheckedChanged += new System.EventHandler(this.IpModeServer_radioButton_CheckedChanged);
            // 
            // IpModeClient_radioButton
            // 
            this.IpModeClient_radioButton.AutoSize = true;
            this.IpModeClient_radioButton.Location = new System.Drawing.Point(4, 42);
            this.IpModeClient_radioButton.Name = "IpModeClient_radioButton";
            this.IpModeClient_radioButton.Size = new System.Drawing.Size(51, 17);
            this.IpModeClient_radioButton.TabIndex = 0;
            this.IpModeClient_radioButton.Text = "Client";
            this.IpModeClient_radioButton.UseVisualStyleBackColor = true;
            this.IpModeClient_radioButton.CheckedChanged += new System.EventHandler(this.IpModeClient_radioButton_CheckedChanged);
            // 
            // IPtype_groupBox
            // 
            this.IPtype_groupBox.Controls.Add(this.CommTypeSerial_radioButton);
            this.IPtype_groupBox.Controls.Add(this.CommTypeUDP_radioButton);
            this.IPtype_groupBox.Controls.Add(this.CommTypeTCP_radioButton);
            this.IPtype_groupBox.Location = new System.Drawing.Point(30, 29);
            this.IPtype_groupBox.Name = "IPtype_groupBox";
            this.IPtype_groupBox.Size = new System.Drawing.Size(72, 87);
            this.IPtype_groupBox.TabIndex = 36;
            this.IPtype_groupBox.TabStop = false;
            this.IPtype_groupBox.Text = "IP Type";
            // 
            // CommTypeSerial_radioButton
            // 
            this.CommTypeSerial_radioButton.AutoSize = true;
            this.CommTypeSerial_radioButton.Location = new System.Drawing.Point(6, 64);
            this.CommTypeSerial_radioButton.Name = "CommTypeSerial_radioButton";
            this.CommTypeSerial_radioButton.Size = new System.Drawing.Size(51, 17);
            this.CommTypeSerial_radioButton.TabIndex = 47;
            this.CommTypeSerial_radioButton.Text = "Serial";
            this.CommTypeSerial_radioButton.UseVisualStyleBackColor = true;
            this.CommTypeSerial_radioButton.CheckedChanged += new System.EventHandler(this.CommTypeSerial_radioButton_CheckedChanged);
            // 
            // CommTypeUDP_radioButton
            // 
            this.CommTypeUDP_radioButton.AutoSize = true;
            this.CommTypeUDP_radioButton.Location = new System.Drawing.Point(6, 42);
            this.CommTypeUDP_radioButton.Name = "CommTypeUDP_radioButton";
            this.CommTypeUDP_radioButton.Size = new System.Drawing.Size(48, 17);
            this.CommTypeUDP_radioButton.TabIndex = 37;
            this.CommTypeUDP_radioButton.Text = "UDP";
            this.CommTypeUDP_radioButton.UseVisualStyleBackColor = true;
            this.CommTypeUDP_radioButton.CheckedChanged += new System.EventHandler(this.IpTypeUDP_radioButton_CheckedChanged);
            // 
            // CommTypeTCP_radioButton
            // 
            this.CommTypeTCP_radioButton.AutoSize = true;
            this.CommTypeTCP_radioButton.Checked = true;
            this.CommTypeTCP_radioButton.Location = new System.Drawing.Point(6, 19);
            this.CommTypeTCP_radioButton.Name = "CommTypeTCP_radioButton";
            this.CommTypeTCP_radioButton.Size = new System.Drawing.Size(46, 17);
            this.CommTypeTCP_radioButton.TabIndex = 0;
            this.CommTypeTCP_radioButton.TabStop = true;
            this.CommTypeTCP_radioButton.Text = "TCP";
            this.CommTypeTCP_radioButton.UseVisualStyleBackColor = true;
            this.CommTypeTCP_radioButton.CheckedChanged += new System.EventHandler(this.IpTypeTCP_radioButton_CheckedChanged);
            // 
            // Debug_tabPage
            // 
            this.Debug_tabPage.BackColor = System.Drawing.SystemColors.Control;
            this.Debug_tabPage.Controls.Add(this.JoyStickDebug_checkBox);
            this.Debug_tabPage.Controls.Add(this.TextBoxClear_button);
            this.Debug_tabPage.Controls.Add(this.debug_textBox);
            this.Debug_tabPage.Controls.Add(this.IpDebug_checkBox);
            this.Debug_tabPage.Location = new System.Drawing.Point(4, 22);
            this.Debug_tabPage.Name = "Debug_tabPage";
            this.Debug_tabPage.Padding = new System.Windows.Forms.Padding(3);
            this.Debug_tabPage.Size = new System.Drawing.Size(349, 373);
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
            this.JoyStickDebug_checkBox.CheckedChanged += new System.EventHandler(this.JoyStickDebug_checkBox_CheckedChanged);
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
            this.About_tabPage.Size = new System.Drawing.Size(349, 373);
            this.About_tabPage.TabIndex = 4;
            this.About_tabPage.Text = "About";
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
            // HouseKeeping_1sec_timer
            // 
            this.HouseKeeping_1sec_timer.Enabled = true;
            this.HouseKeeping_1sec_timer.Interval = 1000;
            this.HouseKeeping_1sec_timer.Tick += new System.EventHandler(this.HouseKeeping_1sec_timer_Tick);
            // 
            // Housekeeping_timer
            // 
            this.Housekeeping_timer.Enabled = true;
            this.Housekeeping_timer.Tick += new System.EventHandler(this.Housekeeping_timer_Tick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnablePreventFocusChange;
            this.ClientSize = new System.Drawing.Size(375, 411);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "UAV Dev Board \"Fly-By-Wire\"";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.Aileron_trackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Elevator_trackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Rudder_trackBar)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.Joy_tabPage.ResumeLayout(false);
            this.Joy_tabPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ThrottleTrim_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RudderTrim_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ElevatorTrim_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AileronTrim_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Throttle_trackBar)).EndInit();
            this.Connection_tabPage.ResumeLayout(false);
            this.Connection_tabPage.PerformLayout();
            this.IP_groupBox.ResumeLayout(false);
            this.IP_groupBox.PerformLayout();
            this.Serial_groupBox.ResumeLayout(false);
            this.Serial_groupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.UpldateRate_numericUpDown)).EndInit();
            this.IPmode_groupBox.ResumeLayout(false);
            this.IPmode_groupBox.PerformLayout();
            this.IPtype_groupBox.ResumeLayout(false);
            this.IPtype_groupBox.PerformLayout();
            this.Debug_tabPage.ResumeLayout(false);
            this.Debug_tabPage.PerformLayout();
            this.About_tabPage.ResumeLayout(false);
            this.About_tabPage.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox debug_textBox;
        private System.Windows.Forms.RadioButton IsConnected_radioButton;
        private System.Windows.Forms.Timer Joystick_timer;
        private System.Windows.Forms.TrackBar Aileron_trackBar;
        private System.Windows.Forms.TrackBar Elevator_trackBar;
        private System.Windows.Forms.TrackBar Rudder_trackBar;
        private System.Windows.Forms.CheckBox OverrideJoy_checkBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox Port_textBox;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage Debug_tabPage;
        private System.Windows.Forms.TabPage Joy_tabPage;
        private System.Windows.Forms.TabPage Connection_tabPage;
        private System.Windows.Forms.Button JoyReset_button;
        private System.Windows.Forms.Button TextBoxClear_button;
        private System.Windows.Forms.RadioButton CommTypeUDP_radioButton;
        private System.Windows.Forms.RadioButton CommTypeTCP_radioButton;
        private System.Windows.Forms.GroupBox IPtype_groupBox;
        private System.Windows.Forms.GroupBox IPmode_groupBox;
        private System.Windows.Forms.RadioButton IpModeServer_radioButton;
        private System.Windows.Forms.RadioButton IpModeClient_radioButton;
        private System.Windows.Forms.TabPage About_tabPage;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TrackBar Throttle_trackBar;
        private System.Windows.Forms.Timer HouseKeeping_1sec_timer;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button EmailTomPittenger_button;
        private System.Windows.Forms.Timer Housekeeping_timer;
        private System.Windows.Forms.Button ClientDisconnect_button;
        public System.Windows.Forms.CheckBox JoyStickDebug_checkBox;
        public System.Windows.Forms.CheckBox IpDebug_checkBox;
        private System.Windows.Forms.TextBox ClientIP_textBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.CheckBox InvertElevator_checkBox;
        private System.Windows.Forms.ComboBox joystickComboBox;
        private System.Windows.Forms.Label IsJoyFound_label;
        private System.Windows.Forms.ComboBox Mode_comboBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button JoystickSetCenter_button;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label6;
        public System.Windows.Forms.CheckBox Connect_checkBox;
        private System.Windows.Forms.Label UploadRate_label;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.NumericUpDown UpldateRate_numericUpDown;
        private System.Windows.Forms.RadioButton CommTypeSerial_radioButton;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.ComboBox CommSerialPort_comboBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.ComboBox CommSerialBaud_comboBox;
        private System.Windows.Forms.GroupBox Serial_groupBox;
        private System.Windows.Forms.GroupBox IP_groupBox;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.NumericUpDown ThrottleTrim_numericUpDown;
        private System.Windows.Forms.NumericUpDown RudderTrim_numericUpDown;
        private System.Windows.Forms.NumericUpDown ElevatorTrim_numericUpDown;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.NumericUpDown AileronTrim_numericUpDown;
        private System.Windows.Forms.CheckBox InvertThrottle_checkBox;
        private System.Windows.Forms.CheckBox InvertRudder_checkBox;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.CheckBox InvertAileron_checkBox;
    }
}

