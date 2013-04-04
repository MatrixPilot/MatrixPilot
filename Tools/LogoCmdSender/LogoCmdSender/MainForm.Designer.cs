namespace LogoCmdSender
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.Connect_checkBox = new System.Windows.Forms.CheckBox();
            this.ClientIP_textBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.ClientDisconnect_button = new System.Windows.Forms.Button();
            this.IsConnected_radioButton = new System.Windows.Forms.RadioButton();
            this.debug_textBox = new System.Windows.Forms.TextBox();
            this.Cmd_comboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.SubCmd_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.Port_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.CommandMessage_textBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.Send_button = new System.Windows.Forms.Button();
            this.Arg_numericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.UseParam_checkBox = new System.Windows.Forms.CheckBox();
            this.DoFly_checkBox = new System.Windows.Forms.CheckBox();
            this.Argument_label = new System.Windows.Forms.Label();
            this.SubCmd_label = new System.Windows.Forms.Label();
            this.DoFly_label = new System.Windows.Forms.Label();
            this.UseParam_label = new System.Windows.Forms.Label();
            this.Housekeeping1sec_timer = new System.Windows.Forms.Timer(this.components);
            this.IPtype_groupBox = new System.Windows.Forms.GroupBox();
            this.CommTypeSerial_radioButton = new System.Windows.Forms.RadioButton();
            this.CommTypeTCP_radioButton = new System.Windows.Forms.RadioButton();
            this.Serial_groupBox = new System.Windows.Forms.GroupBox();
            this.CommSerialPort_comboBox = new System.Windows.Forms.ComboBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.CommSerialBaud_comboBox = new System.Windows.Forms.ComboBox();
            this.IP_groupBox = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.SubCmd_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Port_numericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Arg_numericUpDown)).BeginInit();
            this.IPtype_groupBox.SuspendLayout();
            this.Serial_groupBox.SuspendLayout();
            this.IP_groupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // Connect_checkBox
            // 
            this.Connect_checkBox.AutoSize = true;
            this.Connect_checkBox.Location = new System.Drawing.Point(184, 191);
            this.Connect_checkBox.Name = "Connect_checkBox";
            this.Connect_checkBox.Size = new System.Drawing.Size(66, 17);
            this.Connect_checkBox.TabIndex = 56;
            this.Connect_checkBox.Text = "Connect";
            this.Connect_checkBox.UseVisualStyleBackColor = true;
            this.Connect_checkBox.CheckedChanged += new System.EventHandler(this.Connect_checkBox_CheckedChanged);
            // 
            // ClientIP_textBox
            // 
            this.ClientIP_textBox.Location = new System.Drawing.Point(59, 19);
            this.ClientIP_textBox.Multiline = true;
            this.ClientIP_textBox.Name = "ClientIP_textBox";
            this.ClientIP_textBox.Size = new System.Drawing.Size(169, 24);
            this.ClientIP_textBox.TabIndex = 54;
            this.ClientIP_textBox.Text = "192.168.1.201";
            this.ClientIP_textBox.TextChanged += new System.EventHandler(this.ClientIP_textBox_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(24, 52);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
            this.label2.TabIndex = 53;
            this.label2.Text = "Port:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(11, 22);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(46, 13);
            this.label9.TabIndex = 55;
            this.label9.Text = "UDB IP:";
            // 
            // ClientDisconnect_button
            // 
            this.ClientDisconnect_button.Location = new System.Drawing.Point(16, 191);
            this.ClientDisconnect_button.Name = "ClientDisconnect_button";
            this.ClientDisconnect_button.Size = new System.Drawing.Size(75, 27);
            this.ClientDisconnect_button.TabIndex = 51;
            this.ClientDisconnect_button.Text = "Disconnect";
            this.ClientDisconnect_button.UseVisualStyleBackColor = true;
            this.ClientDisconnect_button.Click += new System.EventHandler(this.ClientDisconnect_button_Click);
            // 
            // IsConnected_radioButton
            // 
            this.IsConnected_radioButton.AutoSize = true;
            this.IsConnected_radioButton.Enabled = false;
            this.IsConnected_radioButton.Location = new System.Drawing.Point(184, 209);
            this.IsConnected_radioButton.Name = "IsConnected_radioButton";
            this.IsConnected_radioButton.Size = new System.Drawing.Size(77, 17);
            this.IsConnected_radioButton.TabIndex = 50;
            this.IsConnected_radioButton.Text = "Connected";
            this.IsConnected_radioButton.UseVisualStyleBackColor = true;
            this.IsConnected_radioButton.CheckedChanged += new System.EventHandler(this.IsConnected_radioButton_CheckedChanged);
            // 
            // debug_textBox
            // 
            this.debug_textBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.debug_textBox.Location = new System.Drawing.Point(270, 10);
            this.debug_textBox.Multiline = true;
            this.debug_textBox.Name = "debug_textBox";
            this.debug_textBox.ReadOnly = true;
            this.debug_textBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.debug_textBox.Size = new System.Drawing.Size(578, 395);
            this.debug_textBox.TabIndex = 57;
            this.debug_textBox.Text = resources.GetString("debug_textBox.Text");
            this.debug_textBox.WordWrap = false;
            // 
            // Cmd_comboBox
            // 
            this.Cmd_comboBox.FormattingEnabled = true;
            this.Cmd_comboBox.Items.AddRange(new object[] {
            "_REPEAT(n, pr)",
            "_END",
            "_ELSE",
            "_TO(fn)",
            "_DO(fn, x, pr)",
            "_EXEC(fn, x, pr)",
            " _FD(x, fl, pr)",
            "_RT(x, pr)",
            "_SET_ANGLE(x, pr)",
            "_USE_CURRENT_ANGLE",
            "_USE_ANGLE_TO_GOAL",
            " _MV_X(x, fl, pr)",
            "_SET_X(x, fl, pr)",
            "_MV_Y(y, fl, pr)",
            "_SET_Y(y, fl, pr)",
            "_MV_Z(z, fl, pr)",
            "_SET_Z(z, fl, pr)",
            "_USE_CURRENT_POS(fl)",
            "_HOME(fl)",
            "_SET_ABS_VAL_HIGH(x)",
            "_SET_ABS_X_LOW(x)",
            "_SET_ABS_Y_LOW(y, fl)",
            "_FLAG_ON(f)",
            "_FLAG_OFF(f)",
            "_FLAG_TOGGLE(f)",
            "_PEN_UP",
            "_PEN_DOWN",
            "_PEN_TOGGLE",
            "_SET_TURTLE(x)",
            "_PARAM_SET(x)",
            "_PARAM_ADD(x)",
            "_PARAM_MUL(x)",
            "_PARAM_DIV(x)",
            "_SPEED_INCREASE(s, pr)",
            "_SET_SPEED(s, pr)",
            "_SET_INTERRUPT(fn)",
            "_CLEAR_INTERRUPT",
            "_LOAD_TO_PARAM(val)",
            "_IF_EQ(val, x, pr)",
            "_IF_NE(val, x, pr)",
            "_IF_GT(val, x, pr)",
            "_IF_LT(val, x, pr)",
            " _IF_GE(val, x, pr)",
            "_IF_LE(val, x, pr)",
            "FD(x)",
            "BK(x)",
            "FD_PARAM",
            "BK_PARAM",
            "RT(x)",
            "LT(x)",
            "SET_ANGLE(x)",
            "RT_PARAM",
            "LT_PARAM",
            "SET_ANGLE_PARAM",
            "USE_CURRENT_ANGLE",
            "USE_ANGLE_TO_GOAL",
            "EAST(x)",
            "WEST(x)",
            "SET_X_POS(x)",
            "EAST_PARAM",
            "WEST_PARAM",
            "SET_X_POS_PARAM",
            "USE_CURRENT_POS",
            "NORTH(y)",
            "SOUTH(y)",
            "SET_Y_POS(y)",
            "NORTH_PARAM",
            "SOUTH_PARAM",
            "SET_Y_POS_PARAM",
            "ALT_UP(z)",
            "ALT_DOWN(z)",
            "SET_ALT(z)",
            "ALT_UP_PARAM",
            "ALT_DOWN_PARAM",
            "SET_ALT_PARAM",
            "SPEED_INCREASE(x)",
            "SPEED_DECREASE(x)",
            "SET_SPEED(x)",
            "SPEED_INCREASE_PARAM",
            "SPEED_DECREASE_PARAM",
            "SET_SPEED_PARAM",
            "FLAG_ON(f)",
            "FLAG_OFF(f)",
            "FLAG_TOGGLE(f)",
            "PEN_UP",
            "PEN_DOWN",
            "PEN_TOGGLE",
            "SET_TURTLE(x)",
            "REPEAT(n)",
            "REPEAT_PARAM",
            "REPEAT_FOREVER",
            "END",
            "ELSE",
            "TO(func)",
            "DO(func)",
            "DO_ARG(func, arg)",
            "DO_PARAM(func)",
            "EXEC(func)",
            "EXEC_ARG(func, arg)",
            "EXEC_PARAM(func)",
            "PARAM_SET(x)",
            "PARAM_ADD(x)",
            "PARAM_SUB(x)",
            "PARAM_MUL(x)",
            "PARAM_DIV(x)",
            "SET_INTERRUPT(fn)",
            "CLEAR_INTERRUPT",
            "LOAD_TO_PARAM(val)",
            "IF_EQ(val, x)",
            "IF_NE(val, x)",
            "IF_GT(val, x)",
            "IF_LT(val, x)",
            "IF_GE(val, x)",
            "IF_LE(val, x)",
            "IF_EQ_PARAM(val)",
            "IF_NE_PARAM(val)",
            "IF_GT_PARAM(val)",
            "IF_LT_PARAM(val)",
            "IF_GE_PARAM(val)",
            "IF_LE_PARAM(val)",
            "SET_POS(x, y)",
            "SET_ABS_POS(x, y)",
            "HOME"});
            this.Cmd_comboBox.Location = new System.Drawing.Point(94, 10);
            this.Cmd_comboBox.Name = "Cmd_comboBox";
            this.Cmd_comboBox.Size = new System.Drawing.Size(170, 21);
            this.Cmd_comboBox.TabIndex = 59;
            this.Cmd_comboBox.SelectedIndexChanged += new System.EventHandler(this.Cmd_comboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(1, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(87, 13);
            this.label1.TabIndex = 62;
            this.label1.Text = "LOGO Command";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(38, 84);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 13);
            this.label3.TabIndex = 63;
            this.label3.Text = "Sub Cmd";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(45, 37);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 13);
            this.label4.TabIndex = 64;
            this.label4.Text = "Do Fly";
            // 
            // SubCmd_numericUpDown
            // 
            this.SubCmd_numericUpDown.Location = new System.Drawing.Point(94, 82);
            this.SubCmd_numericUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.SubCmd_numericUpDown.Name = "SubCmd_numericUpDown";
            this.SubCmd_numericUpDown.Size = new System.Drawing.Size(69, 20);
            this.SubCmd_numericUpDown.TabIndex = 65;
            this.SubCmd_numericUpDown.ValueChanged += new System.EventHandler(this.SubCmd_numericUpDown_ValueChanged);
            // 
            // Port_numericUpDown
            // 
            this.Port_numericUpDown.Location = new System.Drawing.Point(59, 50);
            this.Port_numericUpDown.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.Port_numericUpDown.Name = "Port_numericUpDown";
            this.Port_numericUpDown.Size = new System.Drawing.Size(57, 20);
            this.Port_numericUpDown.TabIndex = 67;
            this.Port_numericUpDown.Value = new decimal(new int[] {
            3005,
            0,
            0,
            0});
            // 
            // CommandMessage_textBox
            // 
            this.CommandMessage_textBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CommandMessage_textBox.Location = new System.Drawing.Point(92, 151);
            this.CommandMessage_textBox.Multiline = true;
            this.CommandMessage_textBox.Name = "CommandMessage_textBox";
            this.CommandMessage_textBox.Size = new System.Drawing.Size(119, 24);
            this.CommandMessage_textBox.TabIndex = 68;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(8, 156);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(83, 13);
            this.label5.TabIndex = 69;
            this.label5.Text = "Outgoing String:";
            // 
            // Send_button
            // 
            this.Send_button.Location = new System.Drawing.Point(217, 149);
            this.Send_button.Name = "Send_button";
            this.Send_button.Size = new System.Drawing.Size(44, 27);
            this.Send_button.TabIndex = 70;
            this.Send_button.Text = "Send";
            this.Send_button.UseVisualStyleBackColor = true;
            this.Send_button.Click += new System.EventHandler(this.Send_button_Click);
            // 
            // Arg_numericUpDown
            // 
            this.Arg_numericUpDown.Location = new System.Drawing.Point(94, 108);
            this.Arg_numericUpDown.Maximum = new decimal(new int[] {
            32767,
            0,
            0,
            0});
            this.Arg_numericUpDown.Minimum = new decimal(new int[] {
            32768,
            0,
            0,
            -2147483648});
            this.Arg_numericUpDown.Name = "Arg_numericUpDown";
            this.Arg_numericUpDown.Size = new System.Drawing.Size(69, 20);
            this.Arg_numericUpDown.TabIndex = 72;
            this.Arg_numericUpDown.ValueChanged += new System.EventHandler(this.Arg_numericUpDown_ValueChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(38, 110);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(52, 13);
            this.label6.TabIndex = 74;
            this.label6.Text = "Argument";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(29, 63);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(59, 13);
            this.label7.TabIndex = 73;
            this.label7.Text = "Use Param";
            // 
            // UseParam_checkBox
            // 
            this.UseParam_checkBox.AutoSize = true;
            this.UseParam_checkBox.Location = new System.Drawing.Point(94, 62);
            this.UseParam_checkBox.Name = "UseParam_checkBox";
            this.UseParam_checkBox.Size = new System.Drawing.Size(15, 14);
            this.UseParam_checkBox.TabIndex = 75;
            this.UseParam_checkBox.UseVisualStyleBackColor = true;
            this.UseParam_checkBox.CheckedChanged += new System.EventHandler(this.UseParam_checkBox_CheckedChanged);
            // 
            // DoFly_checkBox
            // 
            this.DoFly_checkBox.AutoSize = true;
            this.DoFly_checkBox.Location = new System.Drawing.Point(94, 37);
            this.DoFly_checkBox.Name = "DoFly_checkBox";
            this.DoFly_checkBox.Size = new System.Drawing.Size(15, 14);
            this.DoFly_checkBox.TabIndex = 76;
            this.DoFly_checkBox.UseVisualStyleBackColor = true;
            this.DoFly_checkBox.CheckedChanged += new System.EventHandler(this.DoFly_checkBox_CheckedChanged);
            // 
            // Argument_label
            // 
            this.Argument_label.AutoSize = true;
            this.Argument_label.Location = new System.Drawing.Point(169, 110);
            this.Argument_label.Name = "Argument_label";
            this.Argument_label.Size = new System.Drawing.Size(44, 13);
            this.Argument_label.TabIndex = 77;
            this.Argument_label.Text = "<--- ( X )";
            // 
            // SubCmd_label
            // 
            this.SubCmd_label.AutoSize = true;
            this.SubCmd_label.Location = new System.Drawing.Point(169, 84);
            this.SubCmd_label.Name = "SubCmd_label";
            this.SubCmd_label.Size = new System.Drawing.Size(44, 13);
            this.SubCmd_label.TabIndex = 78;
            this.SubCmd_label.Text = "<--- ( X )";
            // 
            // DoFly_label
            // 
            this.DoFly_label.AutoSize = true;
            this.DoFly_label.Location = new System.Drawing.Point(169, 38);
            this.DoFly_label.Name = "DoFly_label";
            this.DoFly_label.Size = new System.Drawing.Size(44, 13);
            this.DoFly_label.TabIndex = 79;
            this.DoFly_label.Text = "<--- ( X )";
            // 
            // UseParam_label
            // 
            this.UseParam_label.AutoSize = true;
            this.UseParam_label.Location = new System.Drawing.Point(169, 62);
            this.UseParam_label.Name = "UseParam_label";
            this.UseParam_label.Size = new System.Drawing.Size(44, 13);
            this.UseParam_label.TabIndex = 80;
            this.UseParam_label.Text = "<--- ( X )";
            // 
            // Housekeeping1sec_timer
            // 
            this.Housekeeping1sec_timer.Interval = 1000;
            this.Housekeeping1sec_timer.Tick += new System.EventHandler(this.Housekeeping1sec_timer_Tick);
            // 
            // IPtype_groupBox
            // 
            this.IPtype_groupBox.Controls.Add(this.CommTypeSerial_radioButton);
            this.IPtype_groupBox.Controls.Add(this.CommTypeTCP_radioButton);
            this.IPtype_groupBox.Location = new System.Drawing.Point(16, 232);
            this.IPtype_groupBox.Name = "IPtype_groupBox";
            this.IPtype_groupBox.Size = new System.Drawing.Size(72, 87);
            this.IPtype_groupBox.TabIndex = 81;
            this.IPtype_groupBox.TabStop = false;
            this.IPtype_groupBox.Text = "IP Type";
            // 
            // CommTypeSerial_radioButton
            // 
            this.CommTypeSerial_radioButton.AutoSize = true;
            this.CommTypeSerial_radioButton.Location = new System.Drawing.Point(11, 20);
            this.CommTypeSerial_radioButton.Name = "CommTypeSerial_radioButton";
            this.CommTypeSerial_radioButton.Size = new System.Drawing.Size(51, 17);
            this.CommTypeSerial_radioButton.TabIndex = 47;
            this.CommTypeSerial_radioButton.Text = "Serial";
            this.CommTypeSerial_radioButton.UseVisualStyleBackColor = true;
            // 
            // CommTypeTCP_radioButton
            // 
            this.CommTypeTCP_radioButton.AutoSize = true;
            this.CommTypeTCP_radioButton.Checked = true;
            this.CommTypeTCP_radioButton.Location = new System.Drawing.Point(11, 50);
            this.CommTypeTCP_radioButton.Name = "CommTypeTCP_radioButton";
            this.CommTypeTCP_radioButton.Size = new System.Drawing.Size(46, 17);
            this.CommTypeTCP_radioButton.TabIndex = 0;
            this.CommTypeTCP_radioButton.TabStop = true;
            this.CommTypeTCP_radioButton.Text = "TCP";
            this.CommTypeTCP_radioButton.UseVisualStyleBackColor = true;
            // 
            // Serial_groupBox
            // 
            this.Serial_groupBox.Controls.Add(this.CommSerialPort_comboBox);
            this.Serial_groupBox.Controls.Add(this.label15);
            this.Serial_groupBox.Controls.Add(this.label16);
            this.Serial_groupBox.Controls.Add(this.CommSerialBaud_comboBox);
            this.Serial_groupBox.Location = new System.Drawing.Point(104, 232);
            this.Serial_groupBox.Name = "Serial_groupBox";
            this.Serial_groupBox.Size = new System.Drawing.Size(146, 87);
            this.Serial_groupBox.TabIndex = 82;
            this.Serial_groupBox.TabStop = false;
            this.Serial_groupBox.Text = "Serial";
            // 
            // CommSerialPort_comboBox
            // 
            this.CommSerialPort_comboBox.FormattingEnabled = true;
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
            // IP_groupBox
            // 
            this.IP_groupBox.Controls.Add(this.ClientIP_textBox);
            this.IP_groupBox.Controls.Add(this.label9);
            this.IP_groupBox.Controls.Add(this.label2);
            this.IP_groupBox.Controls.Add(this.Port_numericUpDown);
            this.IP_groupBox.Location = new System.Drawing.Point(16, 326);
            this.IP_groupBox.Name = "IP_groupBox";
            this.IP_groupBox.Size = new System.Drawing.Size(234, 79);
            this.IP_groupBox.TabIndex = 83;
            this.IP_groupBox.TabStop = false;
            this.IP_groupBox.Text = "IP stuff";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(861, 417);
            this.Controls.Add(this.IP_groupBox);
            this.Controls.Add(this.IsConnected_radioButton);
            this.Controls.Add(this.Serial_groupBox);
            this.Controls.Add(this.IPtype_groupBox);
            this.Controls.Add(this.UseParam_label);
            this.Controls.Add(this.DoFly_label);
            this.Controls.Add(this.SubCmd_label);
            this.Controls.Add(this.Argument_label);
            this.Controls.Add(this.DoFly_checkBox);
            this.Controls.Add(this.UseParam_checkBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.Arg_numericUpDown);
            this.Controls.Add(this.Send_button);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.CommandMessage_textBox);
            this.Controls.Add(this.SubCmd_numericUpDown);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Cmd_comboBox);
            this.Controls.Add(this.debug_textBox);
            this.Controls.Add(this.Connect_checkBox);
            this.Controls.Add(this.ClientDisconnect_button);
            this.Name = "MainForm";
            this.Text = "UAV Dev Board Logo Command Sender - by Tom Pittenger";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.SubCmd_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Port_numericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Arg_numericUpDown)).EndInit();
            this.IPtype_groupBox.ResumeLayout(false);
            this.IPtype_groupBox.PerformLayout();
            this.Serial_groupBox.ResumeLayout(false);
            this.Serial_groupBox.PerformLayout();
            this.IP_groupBox.ResumeLayout(false);
            this.IP_groupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.CheckBox Connect_checkBox;
        private System.Windows.Forms.TextBox ClientIP_textBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button ClientDisconnect_button;
        private System.Windows.Forms.RadioButton IsConnected_radioButton;
        private System.Windows.Forms.TextBox debug_textBox;
        private System.Windows.Forms.ComboBox Cmd_comboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown SubCmd_numericUpDown;
        private System.Windows.Forms.NumericUpDown Port_numericUpDown;
        private System.Windows.Forms.TextBox CommandMessage_textBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button Send_button;
        private System.Windows.Forms.NumericUpDown Arg_numericUpDown;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        public System.Windows.Forms.CheckBox UseParam_checkBox;
        public System.Windows.Forms.CheckBox DoFly_checkBox;
        private System.Windows.Forms.Label Argument_label;
        private System.Windows.Forms.Label SubCmd_label;
        private System.Windows.Forms.Label DoFly_label;
        private System.Windows.Forms.Label UseParam_label;
        private System.Windows.Forms.Timer Housekeeping1sec_timer;
        private System.Windows.Forms.GroupBox IPtype_groupBox;
        private System.Windows.Forms.RadioButton CommTypeSerial_radioButton;
        private System.Windows.Forms.RadioButton CommTypeTCP_radioButton;
        private System.Windows.Forms.GroupBox Serial_groupBox;
        private System.Windows.Forms.ComboBox CommSerialPort_comboBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.ComboBox CommSerialBaud_comboBox;
        private System.Windows.Forms.GroupBox IP_groupBox;
    }
}

